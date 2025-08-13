#include "AbilitySystem/Ability/OpenProjectileSpell.h"
#include "Interfaces/CombatInterface.h"
#include "OpenDemo/Public/OpenGameplayTags.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "Actor/OpenProjectile.h"

void UOpenProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UOpenProjectileSpell::SpawnProjectile(const FRotator& ProjectileRotation, bool bOverridePitch ,float PitchOverride)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	const bool bIsServer = AvatarActor->HasAuthority();
	if (!bIsServer) { return; }

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(AvatarActor);
	if (!CombatInterface) { return; }

	const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
	FRotator Rotation = ProjectileRotation;
	Rotation.Pitch = 0.0f;
	
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}
	
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	
	
	SpawnTransform.SetRotation(Rotation.Quaternion());
	
	// 创建并初始化弹射物
	AOpenProjectile* Projectile = GetWorld()->SpawnActorDeferred<AOpenProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	// 添加一些 GameplayEffect 
	const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AvatarActor);
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	
	const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), SourceASC->MakeEffectContext());

	FOpenGameplayTags GameplayTags = FOpenGameplayTags::Get();

	for (auto& Pair : DamageTypes)
	{
		const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
	}

	Projectile->DamageEffectSpecHandle = SpecHandle;
	
	// 实际生成并发射弹射物
	Projectile->FinishSpawning(SpawnTransform);
}

void UOpenProjectileSpell::AddSpawnNum()
{
	++SpawnNum;
}
