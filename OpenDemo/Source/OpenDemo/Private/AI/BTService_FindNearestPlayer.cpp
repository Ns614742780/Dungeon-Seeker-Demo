#include "AI/BTService_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Enemy/Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/PawnSensingComponent.h"

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* OwningPawn = AIOwner->GetPawn();

	if (OwningPawn == nullptr) { return; }

	const FName TargetTag = OwningPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");

	TArray<AActor*> ActorsWithTag;
	UGameplayStatics::GetAllActorsWithTag(OwningPawn, TargetTag, ActorsWithTag);

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(OwningPawn);
	if (CombatInterface)
	{
		// 如果本来就有目标, 说明已经受击了, 那么直接将攻击来源作为目标
		if (AActor* Target = CombatInterface->Execute_GetCombatTarget(OwningPawn))
		{
			const float Distance = OwningPawn->GetDistanceTo(Target);
				
			UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector, Target);
			UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistanceToTargetSelector, Distance);
			return;
		}
	}
	
	if (UPawnSensingComponent* Sensing = GetPawnSensing(OwnerComp))
	{
		const UWorld* World = GetWorld();
		if (!World) { return; }
		
		for (AActor* Actor : ActorsWithTag)
		{
			if (Sensing->CouldSeePawn(Cast<APawn>(Actor)))
			{
				// 发现了玩家,将发现的 actor 交给 enemy

				const float Distance = OwningPawn->GetDistanceTo(Actor);
				
				UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector, Actor);
				UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistanceToTargetSelector, Distance);
				break;
			}
		}
	}
}

UPawnSensingComponent* UBTService_FindNearestPlayer::GetPawnSensing(const UBehaviorTreeComponent& OwnerComp) const
{
	if (!CachedSensingComponent)
	{
		if (const APawn* Enemy = OwnerComp.GetAIOwner()->GetPawn())
		{
			return Enemy->FindComponentByClass<UPawnSensingComponent>();
		}
		return nullptr;
	}
	return CachedSensingComponent;
}
