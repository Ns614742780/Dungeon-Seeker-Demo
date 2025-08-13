#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_MaxPower.generated.h"

UCLASS()
class OPENDEMO_API UMMC_MaxPower : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_MaxPower();

	// from UGameplayModMagnitudeCalculation
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition StrengthDefinition;
	
};
