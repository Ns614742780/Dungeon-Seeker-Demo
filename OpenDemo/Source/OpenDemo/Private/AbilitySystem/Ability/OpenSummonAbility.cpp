#include "AbilitySystem/Ability/OpenSummonAbility.h"

TArray<FVector> UOpenSummonAbility::GetSpawnLocations()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const float DeltaSpread = SpawnSpread / MinionsCount;

	// the limit of the summon area
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);

	TArray<FVector> SpawnLocations;
	for (int32 i = 0 ; i < MinionsCount; i++)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(i * DeltaSpread, FVector::UpVector);
		FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);

		// do line trance make sure the spawn location is on the ground
		FHitResult HitOnGround;
		GetWorld()->LineTraceSingleByChannel(HitOnGround, ChosenSpawnLocation + FVector(0.f, 0.f, 100.f), ChosenSpawnLocation - FVector(0.f, 0.f, 100.f), ECC_Visibility);

		if (HitOnGround.bBlockingHit)
		{
			ChosenSpawnLocation = HitOnGround.ImpactPoint;
		}
		
		SpawnLocations.Add(ChosenSpawnLocation);
	}
	
	return SpawnLocations;
}

TSubclassOf<APawn> UOpenSummonAbility::GetRandomMinionClass()
{
	int32 Selection = FMath::RandRange(0, MinionClasses.Num() - 1);
	return MinionClasses[Selection];
}

void UOpenSummonAbility::AddSummonCount()
{
	++MinionsCount;
}

void UOpenSummonAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}
