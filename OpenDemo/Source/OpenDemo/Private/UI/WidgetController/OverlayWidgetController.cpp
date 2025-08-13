#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/OpenAttributeSet.h"
#include "AbilitySystem/OpenAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/OpenPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetOpenAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetOpenAS()->GetMaxHealth());
	OnPowerChanged.Broadcast(GetOpenAS()->GetPower());
	OnMaxPowerChanged.Broadcast(GetOpenAS()->GetMaxPower());
}


void UOverlayWidgetController::BindCallbacksToDependencies()
{
	GetOpenPS()->OnPlayerXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnPlayerXPChanged);
	GetOpenPS()->OnPlayerLevelChangedDelegate.AddLambda(
		[this](int32 NewLevel)
		{
			OnLevelChangedDelegate.Broadcast(NewLevel);
		}
	);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetOpenAS()->GetHealthAttribute()).AddLambda(	
			[this](const FOnAttributeChangeData& Data) 
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetOpenAS()->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data) 
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetOpenAS()->GetPowerAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data) 
			{
				OnPowerChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetOpenAS()->GetMaxPowerAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data) 
			{
				OnMaxPowerChanged.Broadcast(Data.NewValue);
			}
		);

	if (GetOpenASC())
	{
		// 如果已经赋予了能力, 那么说明已经广播过了, 那么无需将回调绑定到委托, 直接调用
		if (GetOpenASC()->bStartUpAbilitiesGiven)
		{
			BroadCastAbilityInfo();
		}
		else
		{
			GetOpenASC()->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadCastAbilityInfo);
		}
	
		GetOpenASC()->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for(const FGameplayTag& Tag : AssetTags)
				{
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if (!Tag.MatchesTag(MessageTag)) { continue; }

					const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(
						MessageWidgetDataTable,
						Tag
					);

					MessageWidgetRowDelegate.Broadcast(*Row);
				}
			}
		);
	}
}

void UOverlayWidgetController::OnPlayerXPChanged(int NewXP)
{
	const ULevelUpInfo* LevelUpInfo = GetOpenPS()->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("The AOpenPlayerState Blueprint have not set the LevelUpInfo Data Assert"));

	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();

	if (Level <= MaxLevel && Level > 0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level - 1].LevelUpRequirement;

		const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
		const int32 XPForCurrentLevel = NewXP - PreviousLevelUpRequirement;

		const float XPBarPercent = static_cast<float>(XPForCurrentLevel) / static_cast<float>(DeltaLevelRequirement);

		OnPlayerXPPercentChangedDelegate.Broadcast(XPBarPercent);
	}
}
