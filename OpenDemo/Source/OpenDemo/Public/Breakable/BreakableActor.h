// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

class AOpenEffectActor;
class UGeometryCollectionComponent;
class UCapsuleComponent;

UCLASS()
class OPENDEMO_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABreakableActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// from IHitInterface
	virtual void GetHit_Implementation(const FVector& ImapctPoint, AActor* Hitter) override;
	virtual bool IsItem_Implementation() override;

	// self
	UFUNCTION(BlueprintCallable)
	void SpawnPotion();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UGeometryCollectionComponent* GeometryCollection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCapsuleComponent* Capsule;

private:

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AOpenEffectActor>> PotionClasses;

	bool bIsBroken = false; // Flag to check if the actor is broken

};
