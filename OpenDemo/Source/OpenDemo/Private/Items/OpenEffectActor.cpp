#include "Items/OpenEffectActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/OpenCharacter.h"

AOpenEffectActor::AOpenEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

void AOpenEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AOpenEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) { return; }
	if (!TargetActor->ActorHasTag(FName("Enemy")) && !TargetActor->ActorHasTag(FName("Player"))) { return; }
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (TargetASC == nullptr) {	return;	}

	check(GameplayEffectClass);
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(), TargetASC);

	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && InfiniteEffectRemovePolicy == EEffectRemovePolicy::ERP_RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}

	// has used so destroy (but keep the infinite effect)
	if (!bIsInfinite) { Destroy(); }
}

void AOpenEffectActor::OnOverlap(AActor* TargetActor)
{
	if(TargetActor == nullptr) { return; }
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) { return; }
	if (!TargetActor->ActorHasTag(FName("Enemy")) && !TargetActor->ActorHasTag(FName("Player"))) { return; }
	
	if(InstantEffectApplicationPolicy == EEffectApplicationPolicy::EAP_ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if(DurationEffectApplicationPolicy == EEffectApplicationPolicy::EAP_ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if(InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::EAP_ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AOpenEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if(TargetActor == nullptr) { return; }
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) { return; }
	if (!TargetActor->ActorHasTag(FName("Enemy")) && !TargetActor->ActorHasTag(FName("Player"))) { return; }
	
	if(InstantEffectApplicationPolicy == EEffectApplicationPolicy::EAP_ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if(DurationEffectApplicationPolicy == EEffectApplicationPolicy::EAP_ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if(InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::EAP_ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}

	if(InfiniteEffectRemovePolicy == EEffectRemovePolicy::ERP_RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) { return; }
		
		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		for (const auto& HandlePair : ActiveEffectHandles)
		{
			if (HandlePair.Value == TargetASC)
			{
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key);
				HandlesToRemove.Add(HandlePair.Key);
			}
		}
		for (const auto& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}

	if(bDestroyOnEffectRemove)
	{
		Destroy();
	}
}
