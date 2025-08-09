#include "Input/OpenInputConfig.h"

const UInputAction* UOpenInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FOpenInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find AbilityAction for AbilityTag [%s], on InputConfig [%s]"), *InputTag.ToString(), *GetNameSafe(this))
	}

	return nullptr;
}
