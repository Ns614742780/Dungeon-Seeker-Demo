#include "AbilitySystem/AsyncTask/ListenCooldownChange.h"

#include "AbilitySystemComponent.h"

UListenCooldownChange* UListenCooldownChange::ListenForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent,
                                                                      const FGameplayTag& InCooldownTag)
{
	UListenCooldownChange* ListenCooldownChange = NewObject<UListenCooldownChange>();
	ListenCooldownChange->ASC = AbilitySystemComponent;
	ListenCooldownChange->CooldownTag = InCooldownTag;

	if (!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid())
	{
		ListenCooldownChange->EndTask();
		return nullptr;
	}
	// 值改变时广播, 用于判断冷却时间是否结束
	AbilitySystemComponent->RegisterGameplayTagEvent(
		InCooldownTag,
		EGameplayTagEventType::NewOrRemoved).AddUObject(
			ListenCooldownChange,
			&UListenCooldownChange::CooldownTagChanged);

	// tag 被添加时广播, 在冷却时间触发时通知
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(ListenCooldownChange, &UListenCooldownChange::OnActiveEffectAdded);
	
	return ListenCooldownChange;
}

void UListenCooldownChange::EndTask()
{
	if (!IsValid(ASC)) { return; }

	ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	SetReadyToDestroy();
	MarkAsGarbage();
}

void UListenCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		CooldownEnd.Broadcast(0.f);
	}
}

void UListenCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC,
	const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTag;
	SpecApplied.GetAllGrantedTags(GrantedTag);

	if (AssetTags.HasTagExact(CooldownTag) || GrantedTag.HasTagExact(CooldownTag))
	{
		FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
		TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
		if (TimesRemaining.Num() > 0)
		{
			float TimeRemaining = TimesRemaining[0];
			for (float Time : TimesRemaining)
			{
				if (TimeRemaining < Time)
				{
					TimeRemaining = Time;
				}
			}
			CooldownStart.Broadcast(TimeRemaining);
		}
	}
}
