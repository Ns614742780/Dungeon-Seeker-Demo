#pragma once

#include "CoreMinimal.h"
#include "ConnectRules.h"
#include "GameFramework/Actor.h"
#include "GeneratedMap.generated.h"

class AGameKey;
class AEndDoor;
class AWeapon;
class ABreakableActor;
class ASlot;

/*
 * 方向：
 * 0 -> Front Y
 * 1 -> Right X
 * 2 -> Up	  Z
 * 3 -> Back -Y
 * 4 -> Left -X
 * 5 -> Down -Z
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapGeneratedFinishDelegate, const FVector&, PlayerPoint);

UCLASS(BlueprintType)
class OPENDEMO_API AGeneratedMap : public AActor
{
	GENERATED_BODY()
	
public:	
	AGeneratedMap();

	UPROPERTY(BlueprintAssignable)
	FOnMapGeneratedFinishDelegate OnMapGeneratedFinish;
	
	UPROPERTY(EditAnywhere, Category = "Map")
	FIntVector MapSize;

	UPROPERTY(VisibleAnywhere, Category = "Map")
	FIntVector RealMapSize;

	UPROPERTY(EditAnywhere, Category = "Map|Slot")
	float SlotSize = 800.f;
	
	UPROPERTY(EditAnywhere, Category = "Map|Slot")
	float SlotHeight = 471.f;

	UPROPERTY(EditAnywhere, Category = "Map|Slot")
	float SpawnLightHeightPercent = 0.5;

	UPROPERTY(EditAnywhere, Category = "Map|Slot")
	float SpawnLightRandomRadiusPercent = 0.1;

	UPROPERTY(EditAnywhere, Category = "Map|Slot")
	TArray<TSubclassOf<AActor>> LightClasses;

	UPROPERTY(EditAnywhere, Category = "Map|Slot")
	TArray<TSubclassOf<AActor>> SpawnSmallPropsClasses;

	UPROPERTY(EditAnywhere, Category = "Map|Slot")
	TArray<TSubclassOf<ABreakableActor>> SpawnBreakableClasses;

	UPROPERTY(EditAnywhere, Category = "Map|Weapon")
	TArray<TSubclassOf<AWeapon>> SpawnWeaponClasses;

	UPROPERTY(EditAnywhere, Category = "Map|End")
	TSubclassOf<AEndDoor> EndDoorClass;

	UPROPERTY(EditAnywhere, Category = "Map|End")
	TSubclassOf<AGameKey> GameKeyClass;

	UPROPERTY(EditAnywhere, Category = "Map|Slot")
	int32 EnemyMaxCount = 3;

	UPROPERTY(EditAnywhere, Category = "Map|Slot")
	TArray<int32> SpawnCount;

	UPROPERTY(EditAnywhere, Category = "Map|Slot")
	float EnemyFallOffPercent = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Map|Slot")
	float EnemySpawnAccurate = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Map|Slot")
	float WeaponSpawnAccurate = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Map|Slot")
	float GameKeySpawnAccurate = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Map|Slot")
	float BreakableSpawnRate = 0.3;

	UPROPERTY(EditAnywhere, Category = "Map|Slot")
	float PropsSpawnRate = 0.3;
	
	UPROPERTY(EditAnywhere, Category = "Map|Enemy")
	TArray<TSubclassOf<AActor>> Enemies;
	
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TObjectPtr<UConnectRules> ConnectRules;

	UPROPERTY(EditAnywhere, Category = "Map")
	int32 Seed = 17;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	float AccurateOfStraightRoad = 0.7;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSubclassOf<ASlot> SlotClass;
	
	UPROPERTY(EditAnywhere, Category = "Map")
	bool bDebugMode = false;
	
	TArray<TObjectPtr<ASlot>> Slots;
	TArray<int32> WaitForCollapseSlotsIndex;
	TQueue<int32> SpreadQueueIndex;
	TArray<TArray<FIntVector2>> RoadPointArray;
	TArray<FIntVector2> EndPointArray;

	UFUNCTION(BlueprintCallable)
	void GenerateStart();

	UFUNCTION(BlueprintCallable)
	void SetMapGeneratedPrams(const int32 InX, const int32 InY, const int32 InZ, const int32 InSeed, const float InAccurateOfStraight);
	
	UFUNCTION()
	void InitializeRoads();
	
	UFUNCTION()
	void InitializeMap();

	UFUNCTION()
	void InitializeMapDebug();

	UFUNCTION(BlueprintCallable)
	void CollapseOneStep();
	
	UFUNCTION(BlueprintCallable)
	void CollapseToComplete();
	
	FVector GetPlayerLocation();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	ASlot* GetSlotWithMinEntropy();

	UFUNCTION()
	void CollapseRoad();

	UFUNCTION()
	TArray<int32> GetNeighboringIndexes(const ASlot* InSlot);

	UFUNCTION()
	static int32 GetDirectionByVector(const FIntVector SourceToNeighbor);

	UFUNCTION()
	TArray<FGameplayTag> GetNeedConnectTagsByConnectionRules(const TArray<FGameplayTag>& ConnectionTags) const;

	UFUNCTION()
	void SpreadCollapse(int32 SlotIndex);

	UFUNCTION()
	void SetBoundarySlot(const ASlot* BoundarySlot) const;

	UFUNCTION()
	static void SetDoorByDirection(const ASlot* InSlot, const int32 Direction);

	UFUNCTION()
	static void SetUpRoom(const ASlot* InSlot);

	UFUNCTION()
	static void SetDownRoom(const ASlot* InSlot);

	UFUNCTION()
	static void ForbidUpRoom(const ASlot* InSlot);

	UFUNCTION()
	static void ForbidDownRoom(const ASlot* InSlot);

	UFUNCTION()
	void GenerateNavMeshVolume();

	UFUNCTION()
	void SpawnLight(int32 SlotIndex);

	UFUNCTION()
	void SpawnBreakables(int32 SlotIndex);

	UFUNCTION()
	void SpawnProps(int32 SlotIndex);

	UFUNCTION()
	void SpawnEnemy(int32 SlotIndex);

	UFUNCTION()
	void SpawnWeapon(int32 SlotIndex, int32 WeaponIndex, bool bForceSpawn);

	UFUNCTION()
	void SpawnGameKey(int32 SlotIndex, bool bForceSpawn);

	UFUNCTION()
	void SpawnEndDoor(int32 SlotIndex);
	
	UFUNCTION()
	TArray<FIntVector2> GetRandomRoadPointArray(const int32 Z);

private:
	FORCEINLINE int32 CoordinateToIndex(const FIntVector Pos) const
	{
		return Pos.X + Pos.Y * RealMapSize.X + Pos.Z * RealMapSize.X * RealMapSize.Y;
	}

	int32 WaitForCollapseCount = 0;
	bool bHasKeyGenerated = false;
	bool bHasWeaponGenerated = false;

	FRandomStream RandomStream;
};
