#pragma once

#include "GameplayEffectTypes.h"

#include "OpenAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct  FOpenGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:

	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct(); }

	virtual FOpenGameplayEffectContext* Duplicate() const
	{
		FOpenGameplayEffectContext* NewContext = new FOpenGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}
	
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
	
protected:

	UPROPERTY()
	bool bIsBlockedHit = false;
	UPROPERTY()
	bool bIsCriticalHit = false;


public:
	FORCEINLINE bool IsBlockedHit() const { return bIsBlockedHit; }
	FORCEINLINE bool IsCriticalHit() const { return bIsCriticalHit; }
	FORCEINLINE void SetBlockedHit(const bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
	FORCEINLINE void SetCriticalHit(const bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
};

template<>
struct TStructOpsTypeTraits<FOpenGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FOpenGameplayEffectContext>
{
	enum
	{
		WithNetSerialize = true,
		WithCopy = true
	};
};