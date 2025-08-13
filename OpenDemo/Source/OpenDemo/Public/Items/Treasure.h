// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Treasure.generated.h"

/**
 * 
 */
UCLASS()
class OPENDEMO_API ATreasure : public AItem
{
	GENERATED_BODY()
	

protected:
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
private:
	USoundBase* PickUpSound; // ±¶≤ÿ“Ù–ß

	UPROPERTY(EditAnywhere, Category = "Treasure Properties")
	int32 Gold;

};
