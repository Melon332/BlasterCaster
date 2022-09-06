// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "BlasterCaster/Weapons/Weapon.h"
#include "BlasterCaster/Character/BlasterCharacter.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "BlasterCaster/PlayerController/BlasterPlayerController.h"
#include "Camera/CameraComponent.h"
#include "BlasterCaster/Character/BlasterAnimInstance.h"
#include "Sound/SoundCue.h"
#include "BlasterCaster/Weapons/Projectile.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;

	OffsetX = 15.f;
	OffsetY = 15.f;
}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
		if(Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = Character->GetFollowCamera()->FieldOfView;
		}
		if(Character->HasAuthority())
		{
			InitalizeCarriedAmmo();
		}
	}
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(Character && Character->IsLocallyControlled())
	{
		FHitResult Hit;
		TraceUnderCrossHair(Hit);
		HitTarget = Hit.ImpactPoint;
		
		SetHUDCrosshairs(DeltaTime);
		
		InterpFOV(DeltaTime);
	}
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, SecondaryWeapon);
	DOREPLIFETIME(UCombatComponent, CurrentCombatState);
	DOREPLIFETIME(UCombatComponent, CurrentAmountGrenades);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	if(!Character || !EquippedWeapon) return;
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
	if(Character->IsLocallyControlled() && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_AWP)
	{
		Character->ShowSniperScopeWidget(bIsAiming);
	}
}


void UCombatComponent::OnRep_EquippedWeapon()
{
	if(EquippedWeapon && Character)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

		AttachActorToRightHand(EquippedWeapon);
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;

		PlayEquipWeaponSound(EquippedWeapon);
		EquippedWeapon->ToggleCustomDepth(false);
		EquippedWeapon->UpdateAmmoHUD();
	}
}

void UCombatComponent::OnRep_SecondaryWeapon()
{
	if(Character && SecondaryWeapon)
	{
		AttachActorToBackpack(SecondaryWeapon);
		SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
		PlayEquipWeaponSound(SecondaryWeapon);
	}
}

void UCombatComponent::Fire()
{
	if(CanFire())
	{
		ServerFireButtonPressed(HitTarget);
		LocalFire(HitTarget);
		if(EquippedWeapon)
		{
			CrosshairShootingFactor = CrosshairShootingSpread;
		}
		StartFireTimer();
		bCanFire = false;
	}
}

void UCombatComponent::LocalFire(const FVector_NetQuantize& TraceHitTarget)
{
	if(!EquippedWeapon) return;
	if(Character && CurrentCombatState == ECombatState::ECS_Reloading && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->FireWeapon(TraceHitTarget);
		CurrentCombatState = ECombatState::ECS_Unoccupied;
		return;
	}
	if(Character && CurrentCombatState == ECombatState::ECS_Unoccupied)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->FireWeapon(TraceHitTarget);
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	if(bFireButtonPressed && EquippedWeapon)
	{
		Fire();
	}
}

void UCombatComponent::ServerFireButtonPressed_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFireButtonPressed(TraceHitTarget);
}

void UCombatComponent::MulticastFireButtonPressed_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if(Character && Character->IsLocallyControlled() && !Character->HasAuthority()) return;
	LocalFire(TraceHitTarget);
}


void UCombatComponent::TraceUnderCrossHair(FHitResult& HitResult)
{
	FVector2d ViewportSize;
	if(GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	FVector2D CrosshairLocation(ViewportSize.X / 2.f + OffsetX, ViewportSize.Y / 2.f + OffsetY);
	
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
		);
	
	if(bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;

		if(Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + DistanceToCharacterMax);
		}
		
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;
		
		GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);

		if(HitResult.ImpactPoint == FVector::Zero())
		{
			HitResult.ImpactPoint = End;
		}
	}
	
	if(HitResult.GetActor() && HitResult.GetActor()->Implements<UInteractWithCrosshair>())
	{
		HUDPackage.EnemyColor = EnemyCrosshairColor;
		HasEnemyInSight = true;
	}
	else
	{
		HUDPackage.EnemyColor = DefaultCrosshairColor;
		HasEnemyInSight = false;
	}
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if(!Character || !Character->Controller) return;

	BlasterController = BlasterController == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : BlasterController;

	if(BlasterController)
	{
		HUD = HUD == nullptr ? Cast<ABlasterHUD>(BlasterController->GetHUD()) : HUD;

		if(HUD)
		{
			if(EquippedWeapon)
			{
				HUDPackage.CrosshairBottom = EquippedWeapon->CrosshairBottom;
				HUDPackage.CrosshairCenter = EquippedWeapon->CrosshairCenter;
				HUDPackage.CrosshairLeft = EquippedWeapon->CrosshairLeft;
				HUDPackage.CrosshairRight = EquippedWeapon->CrosshairRight;
				HUDPackage.CrosshairTop = EquippedWeapon->CrosshairTop;
			}
			else
			{
				HUDPackage.CrosshairBottom = nullptr;
				HUDPackage.CrosshairCenter = nullptr;
				HUDPackage.CrosshairLeft = nullptr;
				HUDPackage.CrosshairRight = nullptr;
				HUDPackage.CrosshairTop = nullptr;
			}
			//Calculate crosshair spread

			//600 max walk speed, map speed from [0, 600] to [0, 1]
			FVector2d WalkSpeedRange(0.f,Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2d VelocityMultiplierRange(0,1);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0;
			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());


			if(Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0, DeltaTime, 30);
			}

			if(bAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, CrosshairAimTightness, DeltaTime,30);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0, DeltaTime,30);
			}

			if(HasEnemyInSight)
			{
				CrosshairEnemyInSightFactor = FMath::FInterpTo(CrosshairEnemyInSightFactor, CrosshairAimTightnessEnemy, DeltaTime, 30);
			}
			else
			{
				CrosshairEnemyInSightFactor = FMath::FInterpTo(CrosshairEnemyInSightFactor, 0, DeltaTime, 30);
			}
			
			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0, DeltaTime, 35);
			
			HUDPackage.CrosshairSpread = 0.5f - CrosshairEnemyInSightFactor + CrosshairVelocityFactor + CrosshairInAirFactor- CrosshairAimFactor + CrosshairShootingFactor;
			HUD->SetHUDPackage(HUDPackage);
		}
	}
}

void UCombatComponent::DropEquippedWeapon()
{
	if(EquippedWeapon)
	{
		EquippedWeapon->Dropped();
	}
}

void UCombatComponent::AttachActorToRightHand(AActor* ActorToAttach)
{
	if(!Character || !Character->GetMesh() || !ActorToAttach) return;
	const USkeletalMeshSocket* Socket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if(Socket)
	{
		Socket->AttachActor(ActorToAttach, Character->GetMesh());
	}
}

void UCombatComponent::AttachActorToLeftHand(AActor* ActorToAttach)
{
	if(!Character || !Character->GetMesh() || !ActorToAttach) return;
	const USkeletalMeshSocket* Socket = Character->GetMesh()->GetSocketByName(FName("LeftHandSocket"));
	if(Socket)
	{
		Socket->AttachActor(ActorToAttach, Character->GetMesh());
	}
}

void UCombatComponent::AttachActorToBackpack(AActor* ActorToAttach)
{
	if(!Character || !Character->GetMesh() || !ActorToAttach) return;
	const USkeletalMeshSocket* Socket = Character->GetMesh()->GetSocketByName(FName("SecondaryWeaponSocket"));
	if(Socket)
	{
		Socket->AttachActor(ActorToAttach, Character->GetMesh());
	}
}

void UCombatComponent::UpdateCarriedAmmo()
{
	if(!EquippedWeapon) return;
	if(CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	
	BlasterController = BlasterController == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : BlasterController;
	if(BlasterController)
	{
		BlasterController->SetHUDCarriedAmmo(CarriedAmmo);
	}
}

void UCombatComponent::PlayEquipWeaponSound(AWeapon* EquippingWeapon)
{
	if(Character && EquippingWeapon  && EquippingWeapon->EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquippingWeapon->EquipSound, Character->GetActorLocation());
	}
}

void UCombatComponent::ReloadEmptyWeapon()
{
	if(EquippedWeapon && EquippedWeapon->IsEmpty())
	{
		Reload();
	}
}

void UCombatComponent::EquipPrimaryWeapon(AWeapon* WeaponToEquip)
{
	if(!WeaponToEquip) return;
	DropEquippedWeapon();
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	AttachActorToRightHand(EquippedWeapon);
	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->UpdateAmmoHUD();
	UpdateCarriedAmmo();
	PlayEquipWeaponSound(WeaponToEquip);
	ReloadEmptyWeapon();
}

void UCombatComponent::EquipSecondaryWeapon(AWeapon* WeaponToEquip)
{
	if(!WeaponToEquip) return;
	SecondaryWeapon = WeaponToEquip;
	AttachActorToBackpack(WeaponToEquip);
	SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
	SecondaryWeapon->SetOwner(Character);
	PlayEquipWeaponSound(WeaponToEquip);

	if (SecondaryWeapon == nullptr) return;
	SecondaryWeapon->SetOwner(Character);
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if(!Character || !WeaponToEquip) return;
	if(CurrentCombatState != ECombatState::ECS_Unoccupied) return;

	if(EquippedWeapon && !SecondaryWeapon)
	{
		EquipSecondaryWeapon(WeaponToEquip);
	}
	else
	{
		EquipPrimaryWeapon(WeaponToEquip);
	}
	
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UCombatComponent::SwapWeapons()
{
	if(CurrentCombatState != ECombatState::ECS_Unoccupied) return;
	AWeapon* TempWeapon = EquippedWeapon;
	EquippedWeapon = SecondaryWeapon;
	SecondaryWeapon = TempWeapon;

	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	AttachActorToRightHand(EquippedWeapon);
	EquippedWeapon->UpdateAmmoHUD();
	UpdateCarriedAmmo();
	PlayEquipWeaponSound(EquippedWeapon);
	ReloadEmptyWeapon();

	AttachActorToBackpack(SecondaryWeapon);
	SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
}

void UCombatComponent::Reload()
{
	if(!Character || !EquippedWeapon) return;

	if(EquippedWeapon->GetCurrentAmmo() >= EquippedWeapon->GetMaxMagCapacity()) return;
	if(CarriedAmmo > 0 && CurrentCombatState == ECombatState::ECS_Unoccupied && EquippedWeapon && !EquippedWeapon->IsFull()) 
	{
		ServerReload();
	}
}

void UCombatComponent::FinishReloading()
{
	if(Character == nullptr) return;
	if(Character->HasAuthority())
	{
		CurrentCombatState = ECombatState::ECS_Unoccupied;
		UpdateAmmoValues();
	}
	if(bFireButtonPressed)
	{
		Fire();
	}
}

int32 UCombatComponent::AmountToReload()
{
	if(!EquippedWeapon) return 0;
	int32 RoomInMag = EquippedWeapon->GetMaxMagCapacity() - EquippedWeapon->GetCurrentAmmo();
	if(CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		int32 AmountCarried = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		int32 Least = FMath::Min(RoomInMag,AmountCarried);
		return FMath::Clamp(Least, 0, Least);
	}
	return 0;
}

void UCombatComponent::UpdateAmmoValues()
{
	if(EquippedWeapon == nullptr || Character == nullptr) return;
	int32 ReloadAmount = AmountToReload();
	if(CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	BlasterController = BlasterController == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : BlasterController;
	if(BlasterController)
	{
		BlasterController->SetHUDCarriedAmmo(CarriedAmmo);
	}
	EquippedWeapon->AddAmmo(-ReloadAmount);
}

void UCombatComponent::UpdateShotgunAmmoValues()
{
	if(EquippedWeapon == nullptr || Character == nullptr) return;
	if(CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= 1;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	BlasterController = BlasterController == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : BlasterController;
	if(BlasterController)
	{
		BlasterController->SetHUDCarriedAmmo(CarriedAmmo);
	}
	EquippedWeapon->AddAmmo(-1);
	bCanFire = true;
	if(EquippedWeapon->IsFull() || CarriedAmmo == 0)
	{
		JumpToShotgunEnd();
	}
}

void UCombatComponent::JumpToShotgunEnd()
{
	//Jump to shotgun montage end section
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if(AnimInstance && Character->GetReloadMontage())
	{
		AnimInstance->Montage_JumpToSection(FName("ShotgunEnd"));
	}
}

void UCombatComponent::ShotgunShellReload()
{
	if(Character && Character->HasAuthority())
	{
		UpdateShotgunAmmoValues();
	}
}

void UCombatComponent::ThrowGrenadeFinished()
{
	CurrentCombatState = ECombatState::ECS_Unoccupied;
	if(EquippedWeapon)
	{
		AttachActorToRightHand(EquippedWeapon);
	}
}

void UCombatComponent::LaunchGrenade()
{
	ToggleGrenadeVisiblity(false);
	if(Character && Character->IsLocallyControlled())
	{
		ServerLaunchGrenade(HitTarget);
	}
}

bool UCombatComponent::CanSwapWeapons()
{
	return EquippedWeapon && SecondaryWeapon;
}

void UCombatComponent::PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount)
{
	if(CarriedAmmoMap.Contains(WeaponType))
	{
		CarriedAmmoMap[WeaponType] = FMath::Clamp(CarriedAmmoMap[WeaponType] + AmmoAmount, 0, MaxCarriedAmmo);
		UpdateCarriedAmmo();
	}
	if(EquippedWeapon && EquippedWeapon->IsEmpty() && EquippedWeapon->GetWeaponType() == WeaponType)
	{
		Reload();
	}
}

void UCombatComponent::ServerLaunchGrenade_Implementation(const FVector_NetQuantize& Target)
{
	if(Character && GrenadeClass && Character->GetGrenadeMesh())
	{
		const FVector StartingLocation = Character->GetGrenadeMesh()->GetComponentLocation();
		FVector ToTarget = Target - StartingLocation;
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Character;
		SpawnParams.Instigator = Character;
		if(UWorld* World = GetWorld())
		{
			World->SpawnActor<AProjectile>(GrenadeClass, StartingLocation, ToTarget.Rotation(), SpawnParams);
		}
	}
}

void UCombatComponent::ThrowGrenade()
{
	if(CurrentAmountGrenades <= 0) return;
	if(CurrentCombatState != ECombatState::ECS_Unoccupied || !EquippedWeapon) return;
	CurrentCombatState = ECombatState::ECS_ThrowingGrenade;
	if(Character)
	{
		Character->PlayThrowGrenadeMontage();
		AttachActorToLeftHand(EquippedWeapon);
		ToggleGrenadeVisiblity(true);
	}
	if(Character && !Character->HasAuthority())
	{
		ServerThrowGrenade();
	}
	if(Character && Character->HasAuthority())
	{
		CurrentAmountGrenades = FMath::Clamp(CurrentAmountGrenades - 1, 0, MaxGrenades);
		UpdateHUDGrenades();
	}
}

void UCombatComponent::ServerThrowGrenade_Implementation()
{
	if(CurrentAmountGrenades <= 0) return;
	CurrentCombatState = ECombatState::ECS_ThrowingGrenade;
	if(Character)
	{
		Character->PlayThrowGrenadeMontage();
		AttachActorToLeftHand(EquippedWeapon);
		ToggleGrenadeVisiblity(true);
	}
	CurrentAmountGrenades = FMath::Clamp(CurrentAmountGrenades - 1, 0, MaxGrenades);
	UpdateHUDGrenades();
}

void UCombatComponent::ServerReload_Implementation()
{
	if(!Character || !EquippedWeapon) return;
	
	CurrentCombatState = ECombatState::ECS_Reloading;
	HandleReload();
}

void UCombatComponent::HandleReload()
{
	Character->PlayReloadMontage();
	if(EquippedWeapon)
	{
		EquippedWeapon->PlayReloadWeaponAnimation();
	}
}

void UCombatComponent::OnRep_CombatState()
{
	switch (CurrentCombatState)
	{
	case ECombatState::ECS_Reloading:
		HandleReload();
		break;
	case ECombatState::ECS_Unoccupied:
		if(bFireButtonPressed)
		{
			Fire();
		}
		break;
	case ECombatState::ECS_ThrowingGrenade:
		if(Character && !Character->IsLocallyControlled())
		{
			Character->PlayThrowGrenadeMontage();
			AttachActorToLeftHand(EquippedWeapon);
			ToggleGrenadeVisiblity(true);
		}
		break;
	}
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if(!EquippedWeapon || !Character || !Character->GetFollowCamera()) return;

	if(bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomedInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	
	Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
}

void UCombatComponent::FireTimerFinished()
{
	if(!EquippedWeapon) return;
	
	bCanFire = true;
	if(bFireButtonPressed && EquippedWeapon->GetIsAutomatic() && !EquippedWeapon->IsEmpty())
	{
		Fire();
	}
	ReloadEmptyWeapon();
}

void UCombatComponent::StartFireTimer()
{
	if(EquippedWeapon == nullptr || Character == nullptr) return;
	
	Character->GetWorldTimerManager().SetTimer(FireHandle, this, &ThisClass::FireTimerFinished, EquippedWeapon->GetFireRate());
}

bool UCombatComponent::CanFire() const
{
	if(EquippedWeapon == nullptr || Character == nullptr) return false;
	if(!EquippedWeapon->IsEmpty() && bCanFire && CurrentCombatState == ECombatState::ECS_Reloading && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun) return true;
	return !EquippedWeapon->IsEmpty() && bCanFire && CurrentCombatState == ECombatState::ECS_Unoccupied;
}

void UCombatComponent::ToggleGrenadeVisiblity(bool bVisible)
{
	if(Character && Character->GetGrenadeMesh())
	{
		Character->GetGrenadeMesh()->SetVisibility(bVisible);
	}
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	BlasterController = BlasterController == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : BlasterController;
	if(BlasterController)
	{
		BlasterController->SetHUDCarriedAmmo(CarriedAmmo);
	}
	bool bJumpToShotgunEnd = CurrentCombatState == ECombatState::ECS_Reloading &&
		EquippedWeapon &&
			EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun &&
				CarriedAmmo == 0;
	
	if(bJumpToShotgunEnd)
	{
		JumpToShotgunEnd();
	}
}

void UCombatComponent::OnRep_Grenades()
{
	UpdateHUDGrenades();
}

void UCombatComponent::UpdateHUDGrenades()
{
	BlasterController = BlasterController == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : BlasterController;
	if(BlasterController)
	{
		BlasterController->SetHUDGrenades(CurrentAmountGrenades);
	}
}

void UCombatComponent::InitalizeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, StartingARAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher, StartingRocketAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol, StartingPistolAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SMG, StartingSMGAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Shotgun, StartingShotgunAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AWP, StartingSniperAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_GrenadeLauncher, StartingGrenadeLauncher);
}

