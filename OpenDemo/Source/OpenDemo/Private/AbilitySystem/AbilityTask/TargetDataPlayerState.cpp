#include "AbilitySystem/AbilityTask/TargetDataPlayerState.h"

#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"

UTargetDataPlayerState* UTargetDataPlayerState::CreateTargetDataPlayerState(UGameplayAbility* OwningAbility)
{
	UTargetDataPlayerState* MyObj = NewAbilityTask<UTargetDataPlayerState>(OwningAbility);
	return MyObj;
}

void UTargetDataPlayerState::Activate()
{
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		SendPlayerStateData();
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey PredictionKey = GetActivationPredictionKey();

		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(
			SpecHandle,
			PredictionKey
		).AddUObject(this, &UTargetDataPlayerState::OnTargetDataReplicatedCallback);

		const bool bCalledDelegate = AbilitySystemComponent.Get()->
			CallReplicatedTargetDataDelegatesIfSet(SpecHandle, PredictionKey);

		if (!bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UTargetDataPlayerState::SendPlayerStateData()
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());

	ACharacter* AvatarCharacter = Cast<ACharacter>(Ability->GetCurrentActorInfo()->AvatarActor.Get());
	
	FGameplayAbilityTargetDataHandle TargetDataHandle;

	TArray<TWeakObjectPtr<AActor>> Actors;
	Actors.Add(AvatarCharacter);

	FGameplayAbilityTargetData_ActorArray* TargetData = new FGameplayAbilityTargetData_ActorArray();
	TargetData->SetActors(Actors);

	TargetDataHandle.Add(TargetData);

	FGameplayTag ApplicationTag;

	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		TargetDataHandle,
		ApplicationTag,
		AbilitySystemComponent->ScopedPredictionKey
	);

	if (!ShouldBroadcastAbilityTaskDelegates()) { return; }
	ValidData.Broadcast(TargetDataHandle);
}

void UTargetDataPlayerState::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

	if (!ShouldBroadcastAbilityTaskDelegates()) { return; }
	ValidData.Broadcast(DataHandle);
}