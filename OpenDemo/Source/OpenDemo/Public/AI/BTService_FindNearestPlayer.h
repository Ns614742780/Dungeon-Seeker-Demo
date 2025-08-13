#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "BTService_FindNearestPlayer.generated.h"


class UPawnSensingComponent;

UCLASS()
class OPENDEMO_API UBTService_FindNearestPlayer : public UBTService_BlueprintBase
{
	GENERATED_BODY()

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector TargetToFollowSelector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector DistanceToTargetSelector;
private:
	UPROPERTY(Transient)
	UPawnSensingComponent* CachedSensingComponent;
	
	UPawnSensingComponent* GetPawnSensing(const UBehaviorTreeComponent& OwnerComp) const;
};
