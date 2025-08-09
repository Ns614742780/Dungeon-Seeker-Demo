#include "AbilitySystem/ModMagCalc/MMC_MaxPower.h"

#include "AbilitySystem/OpenAttributeSet.h"
#include "Interfaces/CombatInterface.h"

UMMC_MaxPower::UMMC_MaxPower()
{
	StrengthDefinition.AttributeToCapture = UOpenAttributeSet::GetStrengthAttribute();
	StrengthDefinition.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	StrengthDefinition.bSnapshot = false;

	RelevantAttributesToCapture.Add(StrengthDefinition);
}

float UMMC_MaxPower::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Strength = 0.f;
	GetCapturedAttributeMagnitude(StrengthDefinition, Spec, EvaluationParameters, Strength);
	Strength = FMath::Max(Strength, 0.f);

	int32 PlayerLevel = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}

	return 50.f + 1.5f * Strength + 5.f * PlayerLevel;
}
