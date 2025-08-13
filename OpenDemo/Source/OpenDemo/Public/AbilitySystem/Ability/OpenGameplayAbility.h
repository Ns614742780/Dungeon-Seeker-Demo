#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "OpenGameplayAbility.generated.h"



UCLASS()
class OPENDEMO_API UOpenGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	

public:

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FGameplayTag StartupInputTag;
};
