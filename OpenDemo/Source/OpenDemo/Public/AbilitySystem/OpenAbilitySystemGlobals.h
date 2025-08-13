#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "OpenAbilitySystemGlobals.generated.h"


UCLASS()
class OPENDEMO_API UOpenAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
	
};
