#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "ModuleState.generated.h"

/*
 * 方向：
 * 0 -> Front Y
 * 1 -> Right X
 * 2 -> Up	  Z
 * 3 -> Back -Y
 * 4 -> Left -X
 * 5 -> Down -Z
 */

USTRUCT(BlueprintType)
struct FOpenModuleState
{
	GENERATED_BODY()

	// mesh use to generate the module
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	TObjectPtr<UStaticMesh> Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	int32 Rotation = 0;

	// weight use to calculate the Entropy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	float Weight = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	bool bIsRoom = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	bool bIsRoad = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	bool bSpawnProps = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	FGameplayTag Connect_Left_ID = FGameplayTag();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	FGameplayTag Connect_Right_ID = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	FGameplayTag Connect_Front_ID = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	FGameplayTag Connect_Back_ID = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	FGameplayTag Connect_Up_ID = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	FGameplayTag Connect_Down_ID = FGameplayTag();

	bool operator==(const FOpenModuleState& Other) const
	{
		return Mesh == Other.Mesh && Rotation == Other.Rotation &&
			Connect_Left_ID == Other.Connect_Left_ID && Connect_Right_ID == Other.Connect_Right_ID &&
			Connect_Front_ID == Other.Connect_Front_ID && Connect_Back_ID == Other.Connect_Back_ID &&
			Connect_Up_ID == Other.Connect_Up_ID && Connect_Down_ID == Other.Connect_Down_ID;
	}
	
};

// 哈希函数定义
inline uint32 GetTypeHash(const FOpenModuleState& State)
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(State.Mesh));
	Hash = HashCombine(Hash, GetTypeHash(State.Rotation));
	Hash = HashCombine(Hash, GetTypeHash(State.Connect_Left_ID));
	Hash = HashCombine(Hash, GetTypeHash(State.Connect_Right_ID));
	Hash = HashCombine(Hash, GetTypeHash(State.Connect_Front_ID));
	Hash = HashCombine(Hash, GetTypeHash(State.Connect_Back_ID));
	Hash = HashCombine(Hash, GetTypeHash(State.Connect_Up_ID));
	Hash = HashCombine(Hash, GetTypeHash(State.Connect_Down_ID));
	return Hash;
}

UCLASS()
class OPENDEMO_API UModuleState : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map|BaseData")
	TArray<FOpenModuleState> ModuleStates;

	UFUNCTION()
	static FGameplayTag GetSelfConnectTagByDirection(const FOpenModuleState& ModuleState, const int32 Direction);
	UFUNCTION()
	static FOpenModuleState SetRotation(const FOpenModuleState& ModuleState, const int32 InRotation);

	
};