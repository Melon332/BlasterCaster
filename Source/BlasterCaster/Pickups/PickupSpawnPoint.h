

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupSpawnPoint.generated.h"

UCLASS()
class BLASTERCASTER_API APickupSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	APickupSpawnPoint();
	virtual void Tick(float DeltaTime) override;
	
protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<class APickup>> PickupClasses;

	UPROPERTY()
	APickup* SpawnedPickup;

	void SpawnPickup();
	void SpawnPickupTimerFinished();
	UFUNCTION()
	void StartPickupTimer(AActor* DestroyedActor);
private:

	FTimerHandle SpawnPickupTimer;
	
	UPROPERTY(EditDefaultsOnly)
	float MinSpawnRate;
	UPROPERTY(EditDefaultsOnly)
	float MaxSpawnRate;
};
