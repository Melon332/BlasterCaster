#include "PickupSpawnPoint.h"
#include "Pickup.h"

APickupSpawnPoint::APickupSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
}

void APickupSpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	StartPickupTimer(nullptr);
}


void APickupSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void APickupSpawnPoint::SpawnPickup()
{
	const int32 AmountPickupClasses = PickupClasses.Num();
	if(AmountPickupClasses > 0)
	{
		int32 Selection = FMath::RandRange(0, AmountPickupClasses - 1);
		SpawnedPickup = GetWorld()->SpawnActor<APickup>(PickupClasses[Selection], GetActorTransform());
		if(HasAuthority() && SpawnedPickup)
		{
			SpawnedPickup->OnDestroyed.AddDynamic(this, &ThisClass::StartPickupTimer);
		}
	}
}

void APickupSpawnPoint::SpawnPickupTimerFinished()
{
	if(HasAuthority())
	{
		SpawnPickup();
	}
}

void APickupSpawnPoint::StartPickupTimer(AActor* DestroyedActor)
{
	const float SpawnRate = FMath::RandRange(MinSpawnRate, MaxSpawnRate);
	GetWorld()->GetTimerManager().SetTimer(SpawnPickupTimer, this, &ThisClass::SpawnPickupTimerFinished, SpawnRate);
}

