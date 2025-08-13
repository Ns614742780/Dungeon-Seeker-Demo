#pragma once

#include "CoreMinimal.h"
#include "Module.h"
#include "GameFramework/Actor.h"
#include "ModuleSet.generated.h"

UCLASS()
class OPENDEMO_API AModuleSet : public AActor
{
	GENERATED_BODY()
	
public:	
	AModuleSet();
	
	UPROPERTY(BlueprintReadOnly)
	TSet<FOpenModuleState> MaybeModules;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UModuleState> ModuleStateInfo;

	UFUNCTION()
	void AddModule(TSet<FOpenModuleState> AddModules);
	UFUNCTION()
	void RemoveModule(TSet<FOpenModuleState> RemoveModules);
	UFUNCTION()
	bool RemoveModuleByTagsAndDirection(const TArray<FGameplayTag>& InTags, const int32 Direction);
	bool ForbidModuleByTagsAndDirection(const TArray<FGameplayTag>& InTags, const int32 Direction);
	void Empty();
	void InitMaybeModules();
	UFUNCTION()
	TArray<FGameplayTag> GetConnectTagsByDirection(const int32 Direction);
	
};
