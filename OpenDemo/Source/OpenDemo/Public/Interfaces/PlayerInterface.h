#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};


class OPENDEMO_API IPlayerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	int32 GetXP() const;
	
	UFUNCTION(BlueprintNativeEvent)
	void AddToXP(int32 InXP);

	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForXP(int32 InXP) const;
	
	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePoint() const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPoint() const;

	UFUNCTION(BlueprintNativeEvent)
	void AddToPlayerLevel(int32 InPlayerLevel);

	UFUNCTION(BlueprintNativeEvent)
	void AddToAttributePoints(int32 InAttributePointsNum);

	UFUNCTION(BlueprintNativeEvent)
	void AddToSpellPoints(int32 InSpellPointsNum);
	
	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointReward(int32 InLevel) const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointReward(int32 InLevel) const;
};
