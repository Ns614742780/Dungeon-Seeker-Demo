#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "OpenAIController.generated.h"

class UBehaviorTreeComponent;

UCLASS()
class OPENDEMO_API AOpenAIController : public AAIController
{
	GENERATED_BODY()

public:
	AOpenAIController();


protected:

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
	
};
