#include "AbilitySystem/OpenAbilitySystemGlobals.h"

#include "OpenAbilityTypes.h"

FGameplayEffectContext* UOpenAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FOpenGameplayEffectContext();
}
