#include "UI/WidgetController/OpenWidgetController.h"

#include "AbilitySystem/OpenAbilitySystemComponent.h"
#include "AbilitySystem/OpenAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/OpenPlayerController.h"
#include "Player/OpenPlayerState.h"

void UOpenWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UOpenWidgetController::BroadcastInitialValues()
{
}

void UOpenWidgetController::BindCallbacksToDependencies()
{
}

void UOpenWidgetController::BroadCastAbilityInfo()
{
	if (!GetOpenASC()->bStartUpAbilitiesGiven) { return; }

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda(
		[this](const FGameplayAbilitySpec& AbilitySpec)
		{
			FOpenAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(OpenAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
			Info.InputTag = OpenAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);
			AbilityInfoDelegate.Broadcast(Info);
		}
	);

	GetOpenASC()->ForEachAbility(BroadcastDelegate);
}

AOpenPlayerState* UOpenWidgetController::GetOpenPS()
{
	if (OpenPlayerState == nullptr)
	{
		OpenPlayerState = CastChecked<AOpenPlayerState>(PlayerState);
	}
	return OpenPlayerState;
}

AOpenPlayerController* UOpenWidgetController::GetOpenPC()
{
	if (OpenPlayerController == nullptr)
	{
		OpenPlayerController = CastChecked<AOpenPlayerController>(PlayerController);
	}
	return OpenPlayerController;
}

UOpenAbilitySystemComponent* UOpenWidgetController::GetOpenASC()
{
	if (OpenAbilitySystemComponent == nullptr)
	{
		OpenAbilitySystemComponent = CastChecked<UOpenAbilitySystemComponent>(AbilitySystemComponent);
	}
	return OpenAbilitySystemComponent;
}

UOpenAttributeSet* UOpenWidgetController::GetOpenAS()
{
	if (OpenAttributeSet == nullptr)
	{
		OpenAttributeSet = CastChecked<UOpenAttributeSet>(AttributeSet);
	}
	return OpenAttributeSet;
}
