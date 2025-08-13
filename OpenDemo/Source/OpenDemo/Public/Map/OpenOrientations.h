#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OpenOrientations.generated.h"

UENUM(BlueprintType)
enum class EDirection : uint8
{
	ED_Left = 0,
	ED_Down = 1,
	ED_Back = 2,
	ED_Right = 3,
	ED_Up = 4,
	ED_Front = 5
};

UCLASS(BlueprintType)
class OPENDEMO_API UOpenOrientations : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void Initialize();
	
	UFUNCTION(BlueprintPure, Category = "Orientations")
	static bool IsHorizontal(EDirection Direction);
	
	
};
