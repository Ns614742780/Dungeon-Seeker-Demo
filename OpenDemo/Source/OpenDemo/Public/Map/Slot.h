#pragma once

#include "CoreMinimal.h"
#include "ModuleState.h"
#include "GameFramework/Actor.h"
#include "Slot.generated.h"


class AModuleSet;

UCLASS()
class OPENDEMO_API ASlot : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map|Slot")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	// slot 的坐标
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map|Slot")
	FIntVector MapCoordinate;
	// slot index in array
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map|Slot")
	int32 Index;
	// 熵 用于选择坍缩 slot
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map|Slot")
	float Entropy = 0.f;
	// 当前 slot 是否坍缩
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map|Slot")
	bool bIsCollapsed = false;
	// 坍缩后的当前状态
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map|Slot")
	FOpenModuleState CollapseState;
	// 储存可能状态集合的类
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map|Slot")
	TSubclassOf<AModuleSet> ModuleSetClass;
	// 可能状态的集合
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map|Slot")
	TObjectPtr<AModuleSet> MaybeModuleSet;
	
	ASlot();

	UFUNCTION(BlueprintCallable)
	void InitRandomStream(int32 Seed);
	void CalculateEntropy();
	void Collapse();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	FRandomStream RandomStream;

};
