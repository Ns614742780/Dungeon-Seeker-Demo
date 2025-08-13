#pragma once

#include "CoreMinimal.h"
#include "ModuleState.h"
#include "GameFramework/Actor.h"
#include "Module.generated.h"

/*
 * 方向：
 * 0 -> Front Y
 * 1 -> Right X
 * 2 -> Up	  Z
 * 3 -> Back -Y
 * 4 -> Left -X
 * 5 -> Down -Z
 */


UCLASS()
class OPENDEMO_API AModule : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(BlueprintReadOnly)
	FOpenModuleState ModuleState;
	
	AModule();

	UFUNCTION()
	FGameplayTag GetSelfConnectTagByDirection(const int32 Direction);

private:
	static const TArray<int32>& GetHorizontalDirection();
	bool IsHorizontalDirection(int32 Direction);
	
};
