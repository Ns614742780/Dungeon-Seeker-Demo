#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/OpenAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "OpenGameplayTags.h"
#include "AbilitySystem/OpenAbilitySystemComponent.h"
#include "Player/OpenPlayerState.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	for (auto& Pair : GetOpenAS()->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}
	
	GetOpenPS()->OnAttributePointChangedDelegate.AddLambda(
		[this](int32 InPoints)
		{
			AttributePointChangedDelegate.Broadcast(InPoints);
		}
	);
	
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UOpenAttributeSet* AS = Cast<UOpenAttributeSet>(AttributeSet);
	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
	
	AttributePointChangedDelegate.Broadcast(GetOpenPS()->GetAttributePoint());
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	UOpenAbilitySystemComponent* OpenASC = Cast<UOpenAbilitySystemComponent>(AbilitySystemComponent);
	OpenASC->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	FOpenAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
