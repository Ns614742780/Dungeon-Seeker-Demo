#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerStateTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);

struct FGameplayTag;

UCLASS()
class OPENDEMO_API UTargetDataPlayerState : public UAbilityTask
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Ability|Task", meta = (DisplayName = "TargetDataPlayerState", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UTargetDataPlayerState* CreateTargetDataPlayerState(UGameplayAbility* OwningAbility);

	UPROPERTY(BlueprintAssignable)
	FPlayerStateTargetDataSignature ValidData;


private:

	virtual void Activate() override;
	void SendPlayerStateData();

	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);

};
