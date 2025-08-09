// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};


class OPENDEMO_API IEnemyInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetCombatSpeed();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetPatrolSpeed();
	
};
