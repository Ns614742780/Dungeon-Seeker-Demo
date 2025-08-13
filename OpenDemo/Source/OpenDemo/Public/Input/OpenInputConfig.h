#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "OpenInputConfig.generated.h"


USTRUCT(BlueprintType)
struct FOpenInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	const class UInputAction* InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag();
};


UCLASS()
class OPENDEMO_API UOpenInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:

	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = false) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FOpenInputAction> AbilityInputActions;

};
