#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "UObject/Interface.h"

#include "CombatInterface.generated.h"

class UAnimMontage;

UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class OPENDEMO_API ICombatInterface
{
	GENERATED_BODY()

public:
	virtual FVector GetCombatSocketLocation();

	UFUNCTION(BlueprintNativeEvent)
	int32 GetPlayerLevel();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontage();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FName GetHitReactSectionName(FVector HitPoint);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetAttackMontage();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetAttackSectionCount();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetCombatTarget(AActor* InCombatTarget);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetCombatTarget() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetMinionCount();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddMinionCount(int32 Amount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetAvatar();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	ECharacterClass GetCharacterClass();

	virtual void Die() = 0;
	
};
