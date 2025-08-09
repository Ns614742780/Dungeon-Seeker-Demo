#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/OpenDamageGameplayAbility.h"

#include "OpenProjectileSpell.generated.h"


class AOpenProjectile;
class UGameplayEffect;


UCLASS()
class OPENDEMO_API UOpenProjectileSpell : public UOpenDamageGameplayAbility
{
	GENERATED_BODY()
	

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectile(const FRotator& ProjectileRotation, bool bOverridePitch = false, float PitchOverride = 0.f);

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void AddSpawnNum();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<AOpenProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	int32 SpawnNum = 1;
};
