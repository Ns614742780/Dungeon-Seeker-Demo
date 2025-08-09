#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "ConnectRules.generated.h"

USTRUCT(BlueprintType)
struct FGameplayTagsArray
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FGameplayTag> GameplayTags;
};

UCLASS()
class OPENDEMO_API UConnectRules : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map|ConnectRule")
	TMap<FGameplayTag, FGameplayTagsArray> ConnectRule;

	UFUNCTION()
	FGameplayTagsArray GetConnectTag(FGameplayTag InTag);
	
};
