#pragma once

UENUM(BlueprintType)
enum class EEffectApplicationPolicy : uint8
{
	EAP_ApplyOnOverlap UMETA(DisplayName = "Apply on Overlap"),
	EAP_ApplyOnEndOverlap UMETA(DisplayName = "Apply on End Overlap"),
	EAP_DoNotApply UMETA(DisplayName = "Do Not Apply")
};

UENUM(BlueprintType)
enum class EEffectRemovePolicy : uint8
{
	ERP_RemoveOnEndOverlap UMETA(DisplayName = "Remove on End Overlap"),
	ERP_DoNotRemove UMETA(DisplayName = "Do Not Remove")
};