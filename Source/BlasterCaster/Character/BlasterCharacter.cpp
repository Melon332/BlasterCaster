// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"

#include "BlasterCaster/Components/CombatComponent.h"
#include "BlasterCaster/Weapons/Weapon.h"
#include "Net/UnrealNetwork.h"
#include "UObject/CoreNet.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "BlasterCaster/BlasterCaster.h"
#include "BlasterAnimInstance.h"
#include "BlasterCaster/PlayerController/BlasterPlayerController.h"
#include "BlasterCaster/GameMode/BlasterGameMode.h"
#include "BlasterCaster/PlayerState/BlasterPlayerState.h"
#include "BlasterCaster/Widgets/OverHeadWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "BlasterCaster/Weapons/WeaponTypes.h"

ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0,0,850.f);

	OverHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));
	OverHeadWidget->SetupAttachment(RootComponent);

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
	CombatComponent->SetIsReplicated(true);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);

	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	TurningState = ETurningState::ETIP_NOTTURNING;
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	WalkSpeed = 600.f;
	SprintSpeed = 900.f;

	DissolveTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("Dissolve Timeline"));
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ABlasterCharacter, CurrentHealth);
	DOREPLIFETIME(ABlasterCharacter, bIsRunning);
	DOREPLIFETIME(ABlasterCharacter, bDisableGameplay);
}

void ABlasterCharacter::UpdateHUDHealth()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if(BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDHealth(CurrentHealth, MaxHealth);
	}
}

// Called when the game starts or when spawned
void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	UpdateHUDHealth();

	if(BlasterPlayerController)
	{
		BlasterPlayerController->DeactivateEliminatedText();
	}
	
	if(HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	}
}


// Called every frame
void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateInPlace(DeltaTime);
	PollInit();
	HideCharacterIfCharacterClose();
}

void ABlasterCharacter::RotateInPlace(float DeltaTime)
{
	if(bDisableGameplay)
	{
		TurningState = ETurningState::ETIP_NOTTURNING;
		bUseControllerRotationYaw = true;
		return;
	}
	if(GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementRep += DeltaTime;

		if(TimeSinceLastMovementRep > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
	}
}

// Called to bind functionality to input
void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward / Backward"), this, &ThisClass::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight / Left"), this, &ThisClass::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ThisClass::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp / Down"), this, &ThisClass::LookUp);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this ,&ThisClass::Jump);
	PlayerInputComponent->BindAction(TEXT("Equip"),IE_Pressed,this, &ThisClass::EquipButtonPressed);

	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ThisClass::CrouchButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Crouch"),IE_Released, this, &ThisClass::CrouchButtonReleased);

	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Pressed, this, &ThisClass::AimButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Aim"),IE_Released, this, &ThisClass::AimButtonReleased);

	PlayerInputComponent->BindAction(TEXT("Shoot"), IE_Pressed, this, &ThisClass::FireButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Shoot"), IE_Released, this, &ThisClass::FireButtonReleased);

	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &ThisClass::StartSprinting);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &ThisClass::StopSprinting);

	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &ThisClass::ReloadButtonPressed);
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(CombatComponent)
	{
		CombatComponent->Character = this;
	}
}

void ABlasterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	
	SimProxiesTurn();
	TimeSinceLastMovementRep = 0;
}

void ABlasterCharacter::Destroyed()
{
	Super::Destroyed();

	if(ParticleSystemComponentBot)
	{
		ParticleSystemComponentBot->DestroyComponent();
	}
	ABlasterGameMode* BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	
	bool bMatchNotInProgress = BlasterGameMode && BlasterGameMode->GetMatchState() != MatchState::InProgress;
	
	if(CombatComponent && CombatComponent->EquippedWeapon && bMatchNotInProgress)
	{
		CombatComponent->EquippedWeapon->Destroy();
	}
}

void ABlasterCharacter::FellOutOfWorld(const UDamageType& dmgType)
{
	if(!DiedFromFalling)
	{
		if(ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>())
		{
			BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
			BlasterGameMode->PlayerEliminated(this, BlasterPlayerController, BlasterPlayerController);
			SetDiedFromFalling(true);
		}
	}
}

void ABlasterCharacter::MoveForward(float value)
{
	if(bDisableGameplay) return;
	if(Controller && value != 0)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, value);
	}
}

void ABlasterCharacter::MoveRight(float value)
{
	if(bDisableGameplay) return;
	if(Controller && value != 0)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, value);
	}
}

void ABlasterCharacter::Turn(float value)
{
	AddControllerYawInput(value * Sensitivty);
}

void ABlasterCharacter::LookUp(float value)
{
	AddControllerPitchInput(value * Sensitivty);
}

void ABlasterCharacter::CrouchButtonPressed()
{
	if(bIsCrouched)
	{
		UnCrouch();
		return;
	}
	if(!GetCharacterMovement()->IsFalling())
	{
		Crouch();
	}
}

void ABlasterCharacter::CrouchButtonReleased()
{
	if(UnCrouchOnReleaseCrouchButton)
	{
		UnCrouch();
	}
}

void ABlasterCharacter::AimButtonPressed()
{
	if(bDisableGameplay) return;
	if(CombatComponent && !bIsRunning)
	{
		CombatComponent->SetAiming(true);
	}
}

void ABlasterCharacter::AimButtonReleased()
{
	if(bDisableGameplay) return;
	if(CombatComponent && !bIsRunning)
	{
		CombatComponent->SetAiming(false);
	}
}

void ABlasterCharacter::FireButtonPressed()
{
	if(bDisableGameplay) return;
	if(CombatComponent)
	{
		CombatComponent->FireButtonPressed(true);
	}
}

void ABlasterCharacter::FireButtonReleased()
{
	if(bDisableGameplay) return;
	if(CombatComponent)
	{
		CombatComponent->FireButtonPressed(false);
	}
}

void ABlasterCharacter::ReloadButtonPressed()
{
	if(bDisableGameplay) return;
	if(CombatComponent)
	{
		CombatComponent->Reload();
	}
}

void ABlasterCharacter::EquipButtonPressed()
{
	if(bDisableGameplay) return;
	if(CombatComponent)
	{
		if(HasAuthority())
		{
			CombatComponent->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();
		}
	}
}

void ABlasterCharacter::CalculateAOPitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if(AO_Pitch > 90 && !IsLocallyControlled())
	{
		//Map pitch from the range [270 to 360) to the range [-90 to 0)
		FVector2d InRange(270.f, 360.f);
		FVector2d OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange,OutRange, AO_Pitch);
	}
}

void ABlasterCharacter::CalculateSpeed(float& Speed) const
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;

	Speed = Velocity.Size();
}

void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if(!CombatComponent || CombatComponent->EquippedWeapon == nullptr) return;
	float Speed;
	CalculateSpeed(Speed);

	bool bIsInAir = GetCharacterMovement()->IsFalling();
	if(Speed == 0.f && !bIsInAir)
	{
		//Standing still and not jumping
		FRotator CurrentAimRotation = FRotator(0.f,GetBaseAimRotation().Yaw,0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation,StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if(TurningState == ETurningState::ETIP_NOTTURNING)
		{
			InterpAOYaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;

		bRotateRootBone = true;
		TurnInPlace(DeltaTime);
	}
	if(Speed > 0.f || bIsInAir)
	{
		StartingAimRotation = FRotator(0.f,GetBaseAimRotation().Yaw,0.f);
		AO_Yaw = 0;
		bUseControllerRotationYaw = true;
		TurningState = ETurningState::ETIP_NOTTURNING;
		bRotateRootBone = false;
	}

	CalculateAOPitch();
}

void ABlasterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatorController, AActor* DamageCauser)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);
	PlayHitReactMontage();
	UpdateHUDHealth();

	if(CurrentHealth <= 0.f)
	{
		if(ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>())
		{
			BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
			ABlasterPlayerController* AttackerController = Cast<ABlasterPlayerController>(InstigatorController);
			BlasterGameMode->PlayerEliminated(this, BlasterPlayerController, AttackerController);
		}
	}
}

void ABlasterCharacter::Jump()
{
	if(bDisableGameplay) return;
	if(bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		if(GetCharacterMovement())
		{
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		}
		Super::Jump();
	}
}

void ABlasterCharacter::Landed(const FHitResult& Hit)
{
	if(bIsRunning)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	Super::Landed(Hit);
}

void ABlasterCharacter::PollInit()
{
	if(!BlasterPlayerState)
	{
		BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
		if(BlasterPlayerState)
		{
			BlasterPlayerState->AddToScore(0.f);
			BlasterPlayerState->AddToDefeats(0);
		}
	}
}

void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	if(AO_Yaw > 90)
	{
		TurningState = ETurningState::ETIP_RIGHT;
	}
	else if(AO_Yaw < -90)
	{
		TurningState = ETurningState::ETIP_LEFT;
	}
	if(TurningState != ETurningState::ETIP_NOTTURNING)
	{
		InterpAOYaw = FMath::FInterpTo(InterpAOYaw, 0, DeltaTime, 5.f);
		AO_Yaw = InterpAOYaw;
		if(FMath::Abs(AO_Yaw) < 15)
		{
			TurningState = ETurningState::ETIP_NOTTURNING;
			StartingAimRotation = FRotator(0.f,GetBaseAimRotation().Yaw,0.f);
		}
	}
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickUpWidget(true);
	}
	if(LastWeapon)
	{
		LastWeapon->ShowPickUpWidget(false);
	}
}
void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if(CombatComponent)
	{
		CombatComponent->EquipWeapon(OverlappingWeapon);
	}
}

void ABlasterCharacter::HideCharacterIfCharacterClose()
{
	if(!IsLocallyControlled()) return;

	if((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if(CombatComponent && CombatComponent->EquippedWeapon && CombatComponent->EquippedWeapon->GetWeaponMesh())
		{
			CombatComponent->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if(CombatComponent && CombatComponent->EquippedWeapon && CombatComponent->EquippedWeapon->GetWeaponMesh())
		{
			CombatComponent->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void ABlasterCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if(EliminatedDynamicMaterialInstance)
	{
		EliminatedDynamicMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

void ABlasterCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ThisClass::UpdateDissolveMaterial);
	if(DissolveCurve && DissolveTimeLine)
	{
		DissolveTimeLine->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeLine->Play();
	}
}

void ABlasterCharacter::OnRep_HealthUpdated()
{
	PlayHitReactMontage();
	UpdateHUDHealth();
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* OverlappedWeapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickUpWidget(false);
	}
	
	OverlappingWeapon = OverlappedWeapon;
	if(IsLocallyControlled())
	{
		if(OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickUpWidget(true);
		}
	}
}

bool ABlasterCharacter::IsWeaponEquipped()
{
	return (CombatComponent && CombatComponent->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming()
{
	return CombatComponent && CombatComponent->bAiming;
}

ECombatState ABlasterCharacter::GetCurrentCombatState() const
{
	if(!CombatComponent) return ECombatState::ECS_MAX;
	return CombatComponent->CurrentCombatState;
}

AWeapon* ABlasterCharacter::GetEquippedWeapon()
{
	if(!CombatComponent) return nullptr;
	return CombatComponent->EquippedWeapon;
}

void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if(!CombatComponent || !CombatComponent->EquippedWeapon) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayReloadMontage()
{
	if(!CombatComponent || !CombatComponent->EquippedWeapon) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;
		switch (CombatComponent->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayHitReactMontage()
{
	if(!CombatComponent || !CombatComponent->EquippedWeapon) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName = FName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
		
	}
}

void ABlasterCharacter::SimProxiesTurn()
{
	if(!CombatComponent || !CombatComponent->EquippedWeapon) return;

	bRotateRootBone = false;
	float Speed;
	CalculateSpeed(Speed);
	if(Speed > 0)
	{
		TurningState = ETurningState::ETIP_NOTTURNING;
		return;
	}

	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;
	
	if(FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if(ProxyYaw > TurnThreshold)
		{
			TurningState = ETurningState::ETIP_RIGHT;
		}
		else if(ProxyYaw < -TurnThreshold)
		{
			TurningState = ETurningState::ETIP_LEFT;
		}
		else
		{
			TurningState = ETurningState::ETIP_NOTTURNING;
		}
		return;
	}
	TurningState = ETurningState::ETIP_NOTTURNING;
}

FVector ABlasterCharacter::GetHitTarget() const
{
	if(!CombatComponent) return FVector();

	return CombatComponent->HitTarget;
}

void ABlasterCharacter::ServerSprinting_Implementation(bool bRunning)
{
	bIsRunning = bRunning;

	if(GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = bRunning ? SprintSpeed : WalkSpeed;
	}
}

void ABlasterCharacter::Eliminated()
{
	if(CombatComponent && CombatComponent->EquippedWeapon)
	{
		CombatComponent->EquippedWeapon->Dropped();
		CombatComponent->bFireButtonPressed = false;
	}
	MulticastEliminated();

	GetWorld()->GetTimerManager().SetTimer(EliminatedTimer, this, &ThisClass::EliminatedTimerFinished, EliminatedDelay);
}

void ABlasterCharacter::EliminatedTimerFinished()
{
	if(ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>())
	{
		BlasterGameMode->RequestRespawn(this, Controller);
	}
}

void ABlasterCharacter::StartSprinting()
{
	if(bDisableGameplay) return;
	if(!CombatComponent) return;
	
	if(CombatComponent->bAiming)
	{
		CombatComponent->SetAiming(false);
	}
	CombatComponent->CurrentCombatState = ECombatState::ECS_Sprinting;
	
	bIsRunning = true;
	ServerSprinting(bIsRunning);
	if(GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void ABlasterCharacter::StopSprinting()
{
	if(bDisableGameplay) return;
	if(!CombatComponent) return;
	bIsRunning = false;
	ServerSprinting(bIsRunning);
	if(GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
	CombatComponent->CurrentCombatState = ECombatState::ECS_Unoccupied;

	
	if(CombatComponent->EquippedWeapon && CombatComponent->bFireButtonPressed)
	{
		CombatComponent->Fire();
	}
	
}

void ABlasterCharacter::MulticastEliminated_Implementation()
{
	if(BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDWeaponAmmo(0);
		BlasterPlayerController->SetHUDWeaponName(FString(""), FString(""));
	}
	bEliminated = true;
	PlayElimMontage();

	//Adds the dissolve effect to the character mesh and starts a timer to dissolve
	if(EliminatedMaterialInstance)
	{
		EliminatedDynamicMaterialInstance = UMaterialInstanceDynamic::Create(EliminatedMaterialInstance,this);
		GetMesh()->SetMaterial(0, EliminatedDynamicMaterialInstance);
		EliminatedDynamicMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
		EliminatedDynamicMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 200.f);
		StartDissolve();
	}

	//Disables Character movement
	GetCharacterMovement()->DisableMovement();
	bDisableGameplay = true;
	if(CombatComponent)
	{
		CombatComponent->FireButtonPressed(false);
	}
	
	//Disable Collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Spawns an elimination bot
	if(EliminationBotEffect)
	{
		FVector ElimBotSpawnPoint(GetActorLocation().X,GetActorLocation().Y,GetActorLocation().Z + 200.f);
		ParticleSystemComponentBot = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EliminationBotEffect, ElimBotSpawnPoint, GetActorRotation());
	}
	if(ElimBotSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ElimBotSound, GetActorLocation());
	}
}

