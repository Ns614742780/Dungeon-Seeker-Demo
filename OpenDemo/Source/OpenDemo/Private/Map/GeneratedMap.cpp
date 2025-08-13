#include "Map/GeneratedMap.h"

#include "GroomVisualizationData.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "NavModifierComponent.h"
#include "OpenGameplayTags.h"
#include "Breakable/BreakableActor.h"
#include "Components/BrushComponent.h"
#include "Enemy/Enemy.h"
#include "Items/EndDoor.h"
#include "Items/GameKey.h"
#include "Items/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Map/ModuleSet.h"
#include "Map/Slot.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "OpenDemo/OpenLogChannels.h"
#include "Player/OpenPlayerController.h"

AGeneratedMap::AGeneratedMap()
{
	PrimaryActorTick.bCanEverTick = true;
	RandomStream.Initialize(Seed);
}

void AGeneratedMap::GenerateStart()
{
	InitializeRoads();
	InitializeMapDebug();
}

void AGeneratedMap::SetMapGeneratedPrams(const int32 InX, const int32 InY, const int32 InZ, const int32 InSeed,
	const float InAccurateOfStraight)
{
	MapSize = FIntVector(InX, InY, InZ);
	RandomStream.Initialize(InSeed);
	AccurateOfStraightRoad = InAccurateOfStraight;
}

void AGeneratedMap::InitializeRoads()
{
	// 直接按照格子个数初始化,紧凑安排内存
	RealMapSize = FIntVector(MapSize.X * 2, MapSize.Y * 2, MapSize.Z);

	// 初始化终点位置, 每一层的终点都在起点的对面, 每一层的终点坐标与上一层的起点坐标相同 ( 仅 XY 坐标 )
	if (MapSize.Z > 0)
	{
		EndPointArray.SetNum(MapSize.Z);
		RoadPointArray.SetNum(MapSize.Z);
	}
	for (int32 i = 0; i < MapSize.Z; i++)
	{
		EndPointArray[i].X = RandomStream.RandRange(0, MapSize.Y - 1);
		EndPointArray[i].Y = i % 2 == 0 ? MapSize.Y - 1 : 0;

		// 得到本层终点坐标后, 生成随机路径, 存入路径
		RoadPointArray[i] = GetRandomRoadPointArray(i);
	}
}

void AGeneratedMap::InitializeMap()
{
	const FVector ActorLocation = GetActorLocation();
	const FVector ActorScale = GetActorScale();

	const float ScaleX = SlotSize * ActorScale.X;
	const float ScaleY = SlotSize * ActorScale.Y;
	const float ScaleZ = SlotHeight * ActorScale.Z;
	
	// 直接按照格子个数初始化,紧凑安排内存
	Slots.SetNum(RealMapSize.X * RealMapSize.Y * RealMapSize.Z);
	
	// 高度相对较小,因此放在最外层,尽量保证内存的连续性
	for (int32 Z = 0; Z < RealMapSize.Z; Z++)
	{
		for (int32 Y = 0; Y < RealMapSize.Y; Y++)
		{
			for (int32 X = 0; X < RealMapSize.X; X++)
			{
				FVector SpawnLocation = FVector(ActorLocation.X + X * ScaleX, ActorLocation.Y + Y * ScaleY, ActorLocation.Z + Z * ScaleZ);

				FActorSpawnParameters SpawnParameters;
				SpawnParameters.Owner = this;
				ASlot* NewSlot = GetWorld()->SpawnActor<ASlot>(SlotClass, SpawnLocation, GetActorRotation(), SpawnParameters);
				if (NewSlot)
				{
					NewSlot->MapCoordinate = FIntVector(X,Y,Z);
					NewSlot->Index = CoordinateToIndex(NewSlot->MapCoordinate);
					NewSlot->MaybeModuleSet->InitMaybeModules();
					NewSlot->InitRandomStream(Seed + NewSlot->Index);
					NewSlot->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

					// 设置边界条件, 边界的方块不应该有出口
					if (X == 0 || X == RealMapSize.X - 1 || Y == 0 || Y == RealMapSize.Y - 1 || Z == 0 || Z == RealMapSize.Z - 1)
					{
						SetBoundarySlot(NewSlot);
					}
					NewSlot->CalculateEntropy();
					Slots[NewSlot->Index] = NewSlot;
					WaitForCollapseSlotsIndex.Add(NewSlot->Index);
				}
			}
		}
	}

	GenerateNavMeshVolume();

	WaitForCollapseCount = WaitForCollapseSlotsIndex.Num();
	SpawnCount.SetNum(RealMapSize.Y);
	for (int32 Y = 0 ; Y < RealMapSize.Y; Y++)
	{
		float DistanceToStart = RealMapSize.Y - Y;
		float SpawnLinear = 1.f - DistanceToStart / RealMapSize.Y;
		float SpawnExp = FMath::Pow(SpawnLinear, EnemyFallOffPercent);
		SpawnCount[Y] = FMath::FloorToInt32(SpawnExp * EnemyMaxCount);
	}

	CollapseRoad();

	while (WaitForCollapseCount > 0)
	{
		CollapseOneStep();
	}

	for (const ASlot* Slot : Slots)
	{
		if (Slot->CollapseState.bSpawnProps)
		{
			SpawnProps(Slot->Index);
		}
		if (Slot->CollapseState.bIsRoom)
		{
			SpawnBreakables(Slot->Index);
			SpawnEnemy(Slot->Index);
		}
	}
	UE_LOG(LogOpen, Display, TEXT("[ GeneratedMap ] Map Generation Complete"));
	UE_LOG(LogOpen, Display, TEXT("[ GameMode ] Map Broadcast"));
	if (!bDebugMode)
	{
		OnMapGeneratedFinish.Broadcast(GetPlayerLocation());
	}
}

void AGeneratedMap::InitializeMapDebug()
{
	const FVector ActorLocation = GetActorLocation();
	const FVector ActorScale = GetActorScale();

	const float ScaleX = SlotSize * ActorScale.X;
	const float ScaleY = SlotSize * ActorScale.Y;
	const float ScaleZ = SlotHeight * ActorScale.Z;
	
	Slots.SetNum(RealMapSize.X * RealMapSize.Y * RealMapSize.Z);
	
	// 高度相对较小,因此放在最外层,尽量保证内存的连续性
	for (int32 Z = 0; Z < RealMapSize.Z; Z++)
	{
		for (int32 Y = 0; Y < RealMapSize.Y; Y++)
		{
			for (int32 X = 0; X < RealMapSize.X; X++)
			{
				FVector SpawnLocation = FVector(ActorLocation.X + X * ScaleX, ActorLocation.Y + Y * ScaleY, ActorLocation.Z + Z * ScaleZ);

				FActorSpawnParameters SpawnParameters;
				SpawnParameters.Owner = this;
				ASlot* NewSlot = GetWorld()->SpawnActor<ASlot>(SlotClass, SpawnLocation, GetActorRotation(), SpawnParameters);
				if (NewSlot)
				{
					NewSlot->MapCoordinate = FIntVector(X,Y,Z);
					NewSlot->Index = CoordinateToIndex(NewSlot->MapCoordinate);
					NewSlot->MaybeModuleSet->InitMaybeModules();
					NewSlot->InitRandomStream(Seed + NewSlot->Index);
					NewSlot->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
					
					// 设置边界条件, 边界的方块不应该有出口
					if (X == 0 || X == RealMapSize.X - 1 || Y == 0 || Y == RealMapSize.Y - 1 || Z == 0 || Z == RealMapSize.Z - 1)
					{
						SetBoundarySlot(NewSlot);
					}
					NewSlot->CalculateEntropy();
					Slots[NewSlot->Index] = NewSlot;
					WaitForCollapseSlotsIndex.Add(NewSlot->Index);
				}
			}
		}
	}

	GenerateNavMeshVolume();
	
	WaitForCollapseCount = WaitForCollapseSlotsIndex.Num();
	SpawnCount.SetNum(RealMapSize.Y);
	for (int32 Y = 0 ; Y < RealMapSize.Y; Y++)
	{
		float DistanceToStart = RealMapSize.Y - Y;
		float SpawnLinear = 1.f - DistanceToStart / RealMapSize.Y;
		float SpawnExp = FMath::Pow(SpawnLinear, EnemyFallOffPercent);
		SpawnCount[Y] = FMath::FloorToInt32(SpawnExp * EnemyMaxCount);
	}
	CollapseRoad();
}

void AGeneratedMap::CollapseOneStep()
{
	if (WaitForCollapseCount > 0)
	{
		ASlot* MinEntropySlot = GetSlotWithMinEntropy();
		ForbidUpRoom(MinEntropySlot);
		ForbidDownRoom(MinEntropySlot);
		if (MinEntropySlot == nullptr)
		{
			UE_LOG(LogOpen, Error, TEXT("[ GeneratedMap ] Error: No slot found with min entropy"));
			return;
		}
		MinEntropySlot->Collapse();
		SpawnLight(MinEntropySlot->Index);
		SpreadCollapse(MinEntropySlot->Index);

		// 传播结束, 本次坍缩结束计数 -1
		--WaitForCollapseCount;
	}
	else
	{
		for (const ASlot* Slot : Slots)
		{
			if (Slot->CollapseState.bSpawnProps)
			{
				SpawnProps(Slot->Index);
			}
			if (Slot->CollapseState.bIsRoom)
			{
				SpawnBreakables(Slot->Index);
				SpawnEnemy(Slot->Index);
			}
		}
		UE_LOG(LogOpen, Display, TEXT("[ GeneratedMap ] Map Generation Complete"));
		UE_LOG(LogOpen, Display, TEXT("[ GameMode ] Map Broadcast"));
		if (!bDebugMode)
		{
			OnMapGeneratedFinish.Broadcast(GetPlayerLocation());
		}
	}
}

void AGeneratedMap::CollapseToComplete()
{
	while (WaitForCollapseCount > 0)
	{
		CollapseOneStep();
	}
	CollapseOneStep();
}

void AGeneratedMap::BeginPlay()
{
	Super::BeginPlay();
	// 延迟一帧确保事件先绑定
	
	if (bDebugMode)
	{
		;
	}
	else
	{
		InitializeRoads();
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AGeneratedMap::InitializeMap, 0.1f, false);
	}
}

void AGeneratedMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PC->WasInputKeyJustPressed(EKeys::N))
		{
			CollapseOneStep();
		}
	}
}

void AGeneratedMap::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	Slots.Empty();
	TArray<AActor*> AllActors;
	GetAttachedActors(AllActors);

	for (AActor* Actor : AllActors)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
}

FVector AGeneratedMap::GetPlayerLocation()
{
	FIntVector Coordinate = FIntVector(RoadPointArray[0][0].X, RoadPointArray[0][0].Y, 0);
	int32 StartPointIndex = CoordinateToIndex(Coordinate);
	return Slots[StartPointIndex]->GetActorLocation() + FVector(0, 0, 60);
}


ASlot* AGeneratedMap::GetSlotWithMinEntropy()
{
	if (WaitForCollapseSlotsIndex.Num() == 0) { return nullptr; }
	int32 MinIndexIndex = 0;
	for(int32 i = 0 ; i < WaitForCollapseSlotsIndex.Num() ; i++)
	{
		if (Slots[WaitForCollapseSlotsIndex[i]]->Entropy < Slots[WaitForCollapseSlotsIndex[MinIndexIndex]]->Entropy)
		{
			MinIndexIndex = i;
		}
	}
	// MinIndex 对应的值是最小熵所在的 Slots 的索引值在 WaitForCollapseSlotsIndex 索引数组中的位置, 虽然有点绕, 但这种方式移除性能最好
	const int32 TempIndex = WaitForCollapseSlotsIndex[MinIndexIndex];
	WaitForCollapseSlotsIndex.RemoveAtSwap(MinIndexIndex);
	return Slots[TempIndex];
}

void AGeneratedMap::CollapseRoad()
{
	if (bDebugMode)
	{
		for (int32 i = 0; i < Slots.Num(); i++)
		{
			if (Slots[i])
			{
				DrawDebugPoint(
					GetWorld(),
					Slots[i]->GetActorLocation(),
					10.f,
					FColor::Green,
					true,
					10.f
				);
			}
		}
	}
	
	for (int32 i = 0 ; i < RoadPointArray.Num() ; i++)
	{
		for (int32 j = 0 ; j < RoadPointArray[i].Num() ; j++)
		{
			FIntVector LogicCoordinate = FIntVector(RoadPointArray[i][j].X, RoadPointArray[i][j].Y, 0);
			FIntVector Coordinate = FIntVector(RoadPointArray[i][j].X * 2, RoadPointArray[i][j].Y * 2, i);
			ASlot* RoomSlot = Slots[CoordinateToIndex(Coordinate)];
			FIntVector LastRoomCoordinate;
			FIntVector NextRoomCoordinate;
			int32 LastRoomDirection = -1;
			int32 NextRoomDirection = -1;

			// 由于在边界时清除过所有的出口, 所以在整张地图的出口处恢复出口处 slot 的所有状态
			if (j == RoadPointArray[i].Num() - 1 && i == RoadPointArray.Num() - 1)
			{
				RoomSlot->MaybeModuleSet->InitMaybeModules();
			}
			
			// 该层的中间房间, 此时既有上一个房间又有下一个房间
			if (j > 0 && j < RoadPointArray[i].Num() - 1)
			{
				LastRoomCoordinate = FIntVector(RoadPointArray[i][j - 1].X, RoadPointArray[i][j - 1].Y, 0);
				NextRoomCoordinate = FIntVector(RoadPointArray[i][j + 1].X, RoadPointArray[i][j + 1].Y, 0);

				LastRoomDirection = GetDirectionByVector(LastRoomCoordinate - LogicCoordinate);
				NextRoomDirection = GetDirectionByVector(NextRoomCoordinate - LogicCoordinate);
			}
			else if (j == 0)
			{
				NextRoomCoordinate = FIntVector(RoadPointArray[i][j + 1].X, RoadPointArray[i][j + 1].Y, 0);
				NextRoomDirection = GetDirectionByVector(NextRoomCoordinate - LogicCoordinate);
			}
			else if (j == RoadPointArray[i].Num() - 1)
			{
				LastRoomCoordinate = FIntVector(RoadPointArray[i][j - 1].X, RoadPointArray[i][j - 1].Y, 0);
				LastRoomDirection = GetDirectionByVector(LastRoomCoordinate - LogicCoordinate);
			}

			if (LastRoomDirection != -1)
			{
				SetDoorByDirection(RoomSlot, LastRoomDirection);
			}
			if (NextRoomDirection != -1)
			{
				SetDoorByDirection(RoomSlot, NextRoomDirection);
			}

			FColor Color = (j == RoadPointArray[i].Num() - 1) ? FColor::Red : FColor::Blue;
			// 到该层的终点了, 设置终点的类型为能够向上的房间
			if (j == RoadPointArray[i].Num() - 1)
			{
				SetUpRoom(RoomSlot);
				if (i < RoadPointArray.Num() - 1)
				{
					FIntVector UpCoordinate = Coordinate + FIntVector(0, 0, 1);
					SetDownRoom(Slots[CoordinateToIndex(UpCoordinate)]);
				}
			}	// 其余层不允许有向上的通路与向下的通路
			else
			{
				ForbidUpRoom(RoomSlot);
				// 第一层统一不允许有向下的房间, 除此之外仅允许每层起点有向下的房间
				if (j != 0 || i == 0)
				{
					ForbidDownRoom(RoomSlot);
				}
			}

			RoomSlot->Collapse();
			
			// 在第一层起点放置第一把武器
			if (i == 0)
			{
				if (j == 0)
				{
					SpawnWeapon(RoomSlot->Index, 0, true);
				}
				// 到达了第一层终点
				else if (j == RoadPointArray[i].Num() - 1)
				{
					// 如果第二把武器还没生成
					if (!bHasWeaponGenerated)
					{
						SpawnWeapon(RoomSlot->Index, 1, true);
					}
				}
				else
				{
					if (!bHasWeaponGenerated)
					{
						SpawnWeapon(RoomSlot->Index, 1, false);
					}
				}
			}

			if (!bHasKeyGenerated)
            {
            	SpawnGameKey(RoomSlot->Index, false);
            }
			
			if (i == RoadPointArray.Num() - 1 && j == RoadPointArray[i].Num() - 1)
			{
				if (!bHasKeyGenerated)
				{
					SpawnGameKey(RoomSlot->Index, true);
				}
				SpawnEndDoor(RoomSlot->Index);
			}
			
			SpawnLight(RoomSlot->Index);
			
			SpreadCollapse(RoomSlot->Index);
			--WaitForCollapseCount;
			
			DrawDebugSphere(
				GetWorld(),        // 世界上下文
				RoomSlot->GetActorLocation(),        // 球心位置
				3.0f,           // 半径
				6,         // 细分段数
				Color,            // 颜色
				true, // 是否持续
				1000.f,         // 显示时间（秒）
				0,                // 深度优先级
				2.f         // 线宽
			);
		}
	}
}

TArray<int32> AGeneratedMap::GetNeighboringIndexes(const ASlot* InSlot)
{
	TArray<int32> NeighboringIndexes;
	FIntVector SlotCoordinate = InSlot->MapCoordinate;
	FIntVector NeighboringCoordinate = SlotCoordinate;
	int32 NeighboringIndex;
	
	if (SlotCoordinate.X - 1 >= 0)
	{
		NeighboringCoordinate = SlotCoordinate - FIntVector(1, 0, 0);
		NeighboringIndex = CoordinateToIndex(NeighboringCoordinate);
		if (!Slots[NeighboringIndex]->bIsCollapsed)
		{
			NeighboringIndexes.Add(NeighboringIndex);
		}
	}
	if (SlotCoordinate.X + 1 < RealMapSize.X)
	{
		NeighboringCoordinate = SlotCoordinate + FIntVector(1, 0, 0);
		NeighboringIndex = CoordinateToIndex(NeighboringCoordinate);
		if (!Slots[NeighboringIndex]->bIsCollapsed)
		{
			NeighboringIndexes.Add(NeighboringIndex);
		}
	}
	if (SlotCoordinate.Y - 1 >= 0)
	{
		NeighboringCoordinate = SlotCoordinate - FIntVector(0, 1, 0);
		NeighboringIndex = CoordinateToIndex(NeighboringCoordinate);
		if (!Slots[NeighboringIndex]->bIsCollapsed)
		{
			NeighboringIndexes.Add(NeighboringIndex);
		}
	}
	if (SlotCoordinate.Y + 1 < RealMapSize.Y)
	{
		NeighboringCoordinate = SlotCoordinate + FIntVector(0, 1, 0);
		NeighboringIndex = CoordinateToIndex(NeighboringCoordinate);
		if (!Slots[NeighboringIndex]->bIsCollapsed)
		{
			NeighboringIndexes.Add(NeighboringIndex);
		}
	}
	if (SlotCoordinate.Z - 1 >= 0)
	{
		NeighboringCoordinate = SlotCoordinate - FIntVector(0, 0, 1);
		NeighboringIndex = CoordinateToIndex(NeighboringCoordinate);
		if (!Slots[NeighboringIndex]->bIsCollapsed)
		{
			NeighboringIndexes.Add(NeighboringIndex);
		}
	}
	if (SlotCoordinate.Z + 1 < RealMapSize.Z)
	{
		NeighboringCoordinate = SlotCoordinate + FIntVector(0, 0, 1);
		NeighboringIndex = CoordinateToIndex(NeighboringCoordinate);
		if (!Slots[NeighboringIndex]->bIsCollapsed)
		{
			NeighboringIndexes.Add(NeighboringIndex);
		}
	}

	return NeighboringIndexes;
}

int32 AGeneratedMap::GetDirectionByVector(const FIntVector SourceToNeighbor)
{
	int32 Direction = 0;
	if (SourceToNeighbor == FIntVector(1, 0, 0))
	{
		Direction = 1;
	}
	else if (SourceToNeighbor == FIntVector(-1, 0, 0))
	{
		Direction = 4;
	}
	else if (SourceToNeighbor == FIntVector(0, 1, 0))
	{
		Direction = 0;
	}
	else if (SourceToNeighbor == FIntVector(0, -1, 0))
	{
		Direction = 3;
	}
	else if (SourceToNeighbor == FIntVector(0, 0, 1))
	{
		Direction = 2;
	}
	else if (SourceToNeighbor == FIntVector(0, 0, -1))
	{
		Direction = 5;
	}
	else
	{
		UE_LOG(LogOpen, Display, TEXT("[ GeneratedMap ] Get the wrong direction from source to neighbor"));
	}
	
	return Direction;
}

TArray<FGameplayTag> AGeneratedMap::GetNeedConnectTagsByConnectionRules(const TArray<FGameplayTag>& ConnectionTags) const
{
	TArray<FGameplayTag> NeedConnectionTags;
	for (FGameplayTag ConnectionTag : ConnectionTags)
	{
		FGameplayTagsArray NewConnectionTags = ConnectRules->GetConnectTag(ConnectionTag);
		for (FGameplayTag NewConnectionTag : NewConnectionTags.GameplayTags)
		{
			if (!NeedConnectionTags.Contains(NewConnectionTag))
			{
				NeedConnectionTags.Add(NewConnectionTag);
			}
		}
	}
	return NeedConnectionTags;
}

void AGeneratedMap::SpreadCollapse(int32 SlotIndex)
{
	// 清空队列
	SpreadQueueIndex.Empty();
	TSet<int32> VisitedSlots;
	// 将坍缩的 slot 索引入队等待处理约束的传播
	SpreadQueueIndex.Enqueue(SlotIndex);

	// 如果队列非空, 说明约束传播未完成, 继续传播
	while (!SpreadQueueIndex.IsEmpty())
	{
		// 索引出队列
		int32 SourceIndex = -1;
		SpreadQueueIndex.Dequeue(SourceIndex);
		ASlot* SourceSlot = Slots[SourceIndex];
		
		// 获得未坍缩的邻居
		TArray<int32> NeighboringIndexes = GetNeighboringIndexes(SourceSlot);
		
		// 遍历未坍缩的邻居, 逐个传播约束
		for (const int32 NeighboringIndex : NeighboringIndexes)
		{
			UE_LOG(LogOpen, Display, TEXT("[ GeneratedMap ] Spread Source is [%d](%d, %d, %d)"),Slots[SourceIndex]->Index , Slots[SourceIndex]->MapCoordinate.X, Slots[SourceIndex]->MapCoordinate.Y, Slots[SourceIndex]->MapCoordinate.Z);
			ASlot* NeighboringSlot = Slots[NeighboringIndex];
			UE_LOG(LogOpen, Display, TEXT("[ GeneratedMap ] Spread NeighboringSlot Location is [%d](%d, %d, %d)"), NeighboringSlot->Index, NeighboringSlot->MapCoordinate.X, NeighboringSlot->MapCoordinate.Y, NeighboringSlot->MapCoordinate.Z);
			
			const FIntVector VectorFromSourceToNeighbor = NeighboringSlot->MapCoordinate - SourceSlot->MapCoordinate;
			const int32 DirectionToNeighbor = GetDirectionByVector(VectorFromSourceToNeighbor);
			// 获得相反方向
			const int32 DirectionToSource = (DirectionToNeighbor + 3) % 6;
			// 剩余的可连接 Tag 应当从 SourceSlot 的状态中获得, 也就是谁出队列处理谁
			TArray<FGameplayTag> ConnectionTags = SourceSlot->MaybeModuleSet->GetConnectTagsByDirection(DirectionToNeighbor);
			UE_LOG(LogOpen, Display, TEXT("[ GeneratedMap ] Get Tag Direction is %d"), DirectionToNeighbor);
			for (FGameplayTag LogTag : ConnectionTags)
			{
				UE_LOG(LogOpen, Display, TEXT("[ GeneratedMap ] Get Tag is %s"), *LogTag.ToString());
			}
			
			TArray<FGameplayTag> NeedConnectionTags = GetNeedConnectTagsByConnectionRules(ConnectionTags);
			for (FGameplayTag LogTag : NeedConnectionTags)
			{
				UE_LOG(LogOpen, Display, TEXT("[ GeneratedMap ] Need Tag is %s"), *LogTag.ToString());
			}

			UE_LOG(LogOpen, Display, TEXT("[ GeneratedMap ] Need Tag Direction is %d"), DirectionToSource);
			// 根据连接规则移除邻居模块中的 module, 如果移除了任何 module, 那么就将该邻居加入约束传播的队列
			if (NeighboringSlot->MaybeModuleSet->RemoveModuleByTagsAndDirection(NeedConnectionTags, DirectionToSource))
			{
				SpreadQueueIndex.Enqueue(NeighboringSlot->Index);
				// 如果改变了 ModuleSet, 重新计算熵的值
				NeighboringSlot->CalculateEntropy();
				UE_LOG(LogOpen, Display, TEXT("left modules %d"), NeighboringSlot->MaybeModuleSet->MaybeModules.Num())
				for (FOpenModuleState ModuleState : NeighboringSlot->MaybeModuleSet->MaybeModules)
				{
					UE_LOG(LogOpen, Display, TEXT("%s"), *ModuleState.Mesh.GetFName().ToString())
				}
			}
		}
	}
}

void AGeneratedMap::SetBoundarySlot(const ASlot* BoundarySlot) const
{
	const FIntVector SlotCoordinate = BoundarySlot->MapCoordinate;
	
	TArray<FGameplayTag> BoundaryTags;
	BoundaryTags.Add(FOpenGameplayTags::Get().Map_ConnectID_00);
	BoundaryTags.Add(FOpenGameplayTags::Get().Map_ConnectID_02);
	
	if (SlotCoordinate.X == 0)
	{
		BoundarySlot->MaybeModuleSet->RemoveModuleByTagsAndDirection(BoundaryTags, 4);
	}
	if (SlotCoordinate.X == RealMapSize.X - 1)
	{
		BoundarySlot->MaybeModuleSet->RemoveModuleByTagsAndDirection(BoundaryTags, 1);
	}
	if (SlotCoordinate.Y == 0)
	{
		BoundarySlot->MaybeModuleSet->RemoveModuleByTagsAndDirection(BoundaryTags, 3);
	}
	if (SlotCoordinate.Y == RealMapSize.Y - 1)
	{
		BoundarySlot->MaybeModuleSet->RemoveModuleByTagsAndDirection(BoundaryTags, 0);
	}
	if (SlotCoordinate.Z == 0)
	{
		BoundarySlot->MaybeModuleSet->RemoveModuleByTagsAndDirection(BoundaryTags, 5);
	}
	if (SlotCoordinate.Z == RealMapSize.Z - 1)
	{
		BoundarySlot->MaybeModuleSet->RemoveModuleByTagsAndDirection(BoundaryTags, 2);
	}
}

void AGeneratedMap::SetDoorByDirection(const ASlot* InSlot, const int32 Direction)
{
	TArray<FGameplayTag> DoorTags;
	DoorTags.Add(FOpenGameplayTags::Get().Map_ConnectID_04);
	InSlot->MaybeModuleSet->RemoveModuleByTagsAndDirection(DoorTags, Direction);
}

void AGeneratedMap::SetUpRoom(const ASlot* InSlot)
{
	TArray<FGameplayTag> UpTags;
	UpTags.Add(FOpenGameplayTags::Get().Map_ConnectID_03);
	InSlot->MaybeModuleSet->RemoveModuleByTagsAndDirection(UpTags, 2);
}

void AGeneratedMap::SetDownRoom(const ASlot* InSlot)
{
	TArray<FGameplayTag> DownTags;
	DownTags.Add(FOpenGameplayTags::Get().Map_ConnectID_03);
	InSlot->MaybeModuleSet->RemoveModuleByTagsAndDirection(DownTags, 5);
}

void AGeneratedMap::ForbidUpRoom(const ASlot* InSlot)
{
	TArray<FGameplayTag> UpTags;
	UpTags.Add(FOpenGameplayTags::Get().Map_ConnectID_03);
	InSlot->MaybeModuleSet->ForbidModuleByTagsAndDirection(UpTags, 2);
}

void AGeneratedMap::ForbidDownRoom(const ASlot* InSlot)
{
	TArray<FGameplayTag> DownTags;
	DownTags.Add(FOpenGameplayTags::Get().Map_ConnectID_03);
	InSlot->MaybeModuleSet->ForbidModuleByTagsAndDirection(DownTags, 5);
}

void AGeneratedMap::GenerateNavMeshVolume()
{
	const FVector ActorScale = GetActorScale();
	FVector MinBounds = Slots[0]->GetActorLocation();
	const FVector CenterLocation = Slots[Slots.Num() / 2]->GetActorLocation();
	FVector MaxBounds = Slots[Slots.Num() - 1]->GetActorLocation();

	const float ExtentBound = 800.f;
	MinBounds -= FVector(ExtentBound, ExtentBound, ExtentBound);
	MaxBounds += FVector(ExtentBound, ExtentBound, ExtentBound);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	ANavMeshBoundsVolume* NavMeshBoundsVolume = GetWorld()->SpawnActor<ANavMeshBoundsVolume>(
		ANavMeshBoundsVolume::StaticClass(),
		CenterLocation,
		FRotator::ZeroRotator,
		SpawnParameters
		);
	NavMeshBoundsVolume->GetRootComponent()->SetMobility(EComponentMobility::Stationary);
	NavMeshBoundsVolume->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

	if (NavMeshBoundsVolume)
	{
		NavMeshBoundsVolume->GetRootComponent()->Bounds = FBox(MinBounds, MaxBounds);	
	}
	// 将其添加到导航系统
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSystem)
	{
		NavSystem->OnNavigationBoundsUpdated(NavMeshBoundsVolume); // 通知系统
		NavSystem->Build();
	}
}

void AGeneratedMap::SpawnLight(int32 SlotIndex)
{
	// 不是房间和道路, 无需生成火把
	if (!Slots[SlotIndex]->CollapseState.bIsRoad && !Slots[SlotIndex]->CollapseState.bIsRoom) { return; }

	// 在 Slot 的中心, 高度为 Slot 高度的 50% 的位置做射线检测, 在射线碰到的墙面上生成火把
	FVector SlotCenter = Slots[SlotIndex]->GetActorLocation() + FVector(0, 0, SlotHeight * SpawnLightHeightPercent);

	const TArray<FVector> CastDirections = {FVector::ForwardVector, FVector::RightVector, FVector::BackwardVector, FVector::LeftVector};

	for (const FVector& Direction : CastDirections)
	{
		FVector StartPoint = SlotCenter + Direction * SlotSize * 0.1;
		FVector EndPoint = StartPoint + Direction * SlotSize * 0.6;

		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.bTraceComplex = true;

		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECC_WorldStatic, CollisionParams))
		{
			// 成功检测到墙面，处理该墙面
			// 随机偏移的方向, 是墙面的切线
			const FVector RandomMoveDirection = FVector::CrossProduct(HitResult.ImpactNormal, FVector::UpVector).GetSafeNormal();
			const float Offset = RandomStream.FRandRange(-SpawnLightRandomRadiusPercent * SlotSize, SpawnLightRandomRadiusPercent * SlotSize);
			const FVector SpawnLocation = HitResult.Location + RandomMoveDirection * Offset;
			
			FRotator SpawnRotation = FRotationMatrix::MakeFromX(HitResult.ImpactNormal).Rotator();
			SpawnRotation.Yaw -= 90.f;
			
			TSubclassOf<AActor> TorchClass = LightClasses[RandomStream.FRandRange(0, LightClasses.Num() - 1)];
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = this;
			AActor* Torch = GetWorld()->SpawnActor<AActor>(TorchClass, SpawnLocation, SpawnRotation, SpawnParameters);
			Torch->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		}
		else
		{
			FVector OffsetStartPoint;
			FVector OffsetEndPoint;
			// 当前方向并未发现墙面, 选择与其垂直的方向偏移后再做检测, 生成在门的侧面
			if (Direction == FVector::ForwardVector || Direction == FVector::BackwardVector)
			{
				OffsetStartPoint = SlotCenter + FVector::LeftVector * SlotSize * 0.3 + Direction * SlotSize * 0.1;
				OffsetEndPoint = OffsetStartPoint + Direction * SlotSize;
			}
			else
			{
				OffsetStartPoint = SlotCenter + FVector::ForwardVector * SlotSize * 0.3 + Direction * SlotSize * 0.1;
				OffsetEndPoint = OffsetStartPoint + Direction * SlotSize;
			}
			if (GetWorld()->LineTraceSingleByChannel(HitResult, OffsetStartPoint, OffsetEndPoint, ECC_WorldStatic, CollisionParams))
			{
				// 成功检测到墙面，处理该墙面
				// 随机偏移的方向, 是墙面的切线
				const FVector RandomMoveDirection = FVector::CrossProduct(HitResult.ImpactNormal, FVector::UpVector).GetSafeNormal();
				const float Offset = RandomStream.FRandRange(-SpawnLightRandomRadiusPercent * SlotSize, SpawnLightRandomRadiusPercent * SlotSize);
				const FVector SpawnLocation = HitResult.Location + RandomMoveDirection * Offset;
			
				FRotator SpawnRotation = FRotationMatrix::MakeFromX(HitResult.ImpactNormal).Rotator();
				SpawnRotation.Yaw -= 90.f;
			
				TSubclassOf<AActor> TorchClass = LightClasses[RandomStream.FRandRange(0, LightClasses.Num() - 1)];
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.Owner = this;
				AActor* Torch = GetWorld()->SpawnActor<AActor>(TorchClass, SpawnLocation, SpawnRotation, SpawnParameters);
				Torch->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			}
		}
	}
}

void AGeneratedMap::SpawnBreakables(int32 SlotIndex)
{
	if (!Slots[SlotIndex]->CollapseState.bIsRoom) { return; }
	// 得到房间中心的坐标, 用于选择位置的射线检测
	FVector SlotCenter = Slots[SlotIndex]->GetActorLocation() + FVector(0, 0, SlotHeight * 0.5);
	const TArray<FVector> OffsetDirections = {
		FVector(1.0f,1.0f,0.0f),
		FVector(1.0f,-1.0f,0.0f),
		FVector(-1.0f,1.0f,0.0f),
		FVector(-1.0f,-1.0f,0.0f)
	};

	for (FVector OffsetDirection : OffsetDirections)
	{
		FVector TraceLocation = SlotCenter + OffsetDirection * SlotSize * 0.2;
		if (RandomStream.FRandRange(0.0f, 1.0f) > BreakableSpawnRate) { continue; }
		FVector StartPoint = TraceLocation;
		FVector EndPoint = TraceLocation - FVector(0, 0, SlotHeight);

		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.bTraceComplex = true;

		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECC_WorldStatic, CollisionParams))
		{
			// 从当前坐标随机偏移
			const float OffsetX = RandomStream.FRandRange(-0.05 * SlotSize, 0.05 * SlotSize);
			const float OffsetY = RandomStream.FRandRange(-0.05 * SlotSize, 0.05 * SlotSize);
			const FVector SpawnLocation = HitResult.Location + FVector(OffsetX, OffsetY, 0);
			
			TSubclassOf<ABreakableActor> BreakableClass = SpawnBreakableClasses[RandomStream.FRandRange(0, SpawnBreakableClasses.Num() - 1)];
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = this;
			ABreakableActor* Breakable = GetWorld()->SpawnActor<ABreakableActor>(BreakableClass, SpawnLocation, FRotator::ZeroRotator, SpawnParameters);
			Breakable->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		}
	}
}

void AGeneratedMap::SpawnProps(int32 SlotIndex)
{
	// 不是房间和道路, 无需生成物品
	if (!Slots[SlotIndex]->CollapseState.bIsRoad && !Slots[SlotIndex]->CollapseState.bIsRoom) { return; }
	
	// 在 Slot 的中心, 高度为 Slot 高度的 50% 的位置做射线检测, 有碰撞则说明是墙壁, 沿着碰撞到的地方生成
	FVector SlotCenter = Slots[SlotIndex]->GetActorLocation() + FVector(0, 0, 5);

	const TArray<FVector> CastForwardAndBack = { FVector::ForwardVector, FVector::BackwardVector };
	const TArray<FVector> CastLeftAndRight = {FVector::RightVector, FVector::LeftVector};

	TArray<FVector> ForwardAndBackSourceLocation = {
		SlotCenter + CastForwardAndBack[0] * SlotSize * 0.25,
		SlotCenter + CastForwardAndBack[1] * SlotSize * 0.25
	};
	TArray<FVector> LeftAndRightSourceLocation = {
		SlotCenter + CastLeftAndRight[0] * SlotSize * 0.25,
		SlotCenter + CastLeftAndRight[1] * SlotSize * 0.25
	};

	if (Slots[SlotIndex]->CollapseState.bIsRoom)
	{
		for (const FVector& Location : ForwardAndBackSourceLocation)
		{
			for (const FVector& Direction : CastLeftAndRight)
			{
				if (RandomStream.FRandRange(0.0f, 1.0f) > PropsSpawnRate) { continue; }
				FVector StartPoint = Location + Direction * SlotSize * 0.1;
				FVector EndPoint = StartPoint + Direction * SlotSize * 0.6;
				
				FHitResult HitResult;
				FCollisionQueryParams CollisionParams;
				CollisionParams.bTraceComplex = true;

				if (GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECC_WorldStatic, CollisionParams))
				{
					// 成功检测到墙面，处理该墙面
					// 随机偏移的方向, 是墙面的切线
					const FVector RandomMoveDirection = FVector::CrossProduct(HitResult.ImpactNormal, FVector::UpVector).GetSafeNormal();
					const float Offset = RandomStream.FRandRange(-0.01 * SlotSize, 0.01 * SlotSize);
					const FVector SpawnLocation = HitResult.Location + RandomMoveDirection * Offset - Direction * 80;
				
					FRotator SpawnRotation = FRotationMatrix::MakeFromX(HitResult.ImpactNormal).Rotator();
					SpawnRotation.Yaw -= 90.f;
				
					TSubclassOf<AActor> PropsClass = SpawnSmallPropsClasses[RandomStream.FRandRange(0, SpawnSmallPropsClasses.Num() - 1)];
					FActorSpawnParameters SpawnParameters;
					SpawnParameters.Owner = this;
					AActor* Prop = GetWorld()->SpawnActor<AActor>(PropsClass, SpawnLocation, SpawnRotation, SpawnParameters);
					Prop->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
				}
			}
		}

		for (const FVector& Location : LeftAndRightSourceLocation)
		{
			for (const FVector& Direction : CastForwardAndBack)
			{
				if (RandomStream.FRandRange(0.0f, 1.0f) > PropsSpawnRate) { continue; }
				FVector StartPoint = Location + Direction * SlotSize * 0.1;
				FVector EndPoint = StartPoint + Direction * SlotSize * 0.6;
				
				FHitResult HitResult;
				FCollisionQueryParams CollisionParams;
				CollisionParams.bTraceComplex = true;

				if (GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECC_WorldStatic, CollisionParams))
				{
					// 成功检测到墙面，处理该墙面
					// 随机偏移的方向, 是墙面的切线
					const FVector RandomMoveDirection = FVector::CrossProduct(HitResult.ImpactNormal, FVector::UpVector).GetSafeNormal();
					const float Offset = RandomStream.FRandRange(-0.01 * SlotSize, 0.01 * SlotSize);
					const FVector SpawnLocation = HitResult.Location + RandomMoveDirection * Offset - Direction * 80;
				
					FRotator SpawnRotation = FRotationMatrix::MakeFromX(HitResult.ImpactNormal).Rotator();
					SpawnRotation.Yaw -= 90.f;
				
					TSubclassOf<AActor> PropsClass = SpawnSmallPropsClasses[RandomStream.FRandRange(0, SpawnSmallPropsClasses.Num() - 1)];
					FActorSpawnParameters SpawnParameters;
					SpawnParameters.Owner = this;
					AActor* Prop = GetWorld()->SpawnActor<AActor>(PropsClass, SpawnLocation, SpawnRotation, SpawnParameters);
					Prop->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
				}
			}
		}
	}
	if (Slots[SlotIndex]->CollapseState.bIsRoad)
	{
		for (const FVector& Location : ForwardAndBackSourceLocation)
		{
			if (RandomStream.FRandRange(0.0f, 1.0f) > PropsSpawnRate) { continue; }
			const FVector& Direction = CastLeftAndRight[0];
			FVector StartPoint = Location + Direction * SlotSize * 0.1;
			FVector EndPoint = StartPoint + Direction * SlotSize * 0.6;
			
			FHitResult HitResult;
			FCollisionQueryParams CollisionParams;
			CollisionParams.bTraceComplex = true;

			if (GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECC_WorldStatic, CollisionParams))
			{
				// 成功检测到墙面，处理该墙面
				// 随机偏移的方向, 是墙面的切线
				const FVector RandomMoveDirection = FVector::CrossProduct(HitResult.ImpactNormal, FVector::UpVector).GetSafeNormal();
				const float Offset = RandomStream.FRandRange(-0.01 * SlotSize, 0.01 * SlotSize);
				const FVector SpawnLocation = HitResult.Location + RandomMoveDirection * Offset - Direction * 80;
			
				FRotator SpawnRotation = FRotationMatrix::MakeFromX(HitResult.ImpactNormal).Rotator();
				SpawnRotation.Yaw -= 90.f;
			
				TSubclassOf<AActor> PropsClass = SpawnSmallPropsClasses[RandomStream.FRandRange(0, SpawnSmallPropsClasses.Num() - 1)];
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.Owner = this;
				AActor* Prop = GetWorld()->SpawnActor<AActor>(PropsClass, SpawnLocation, SpawnRotation, SpawnParameters);
				Prop->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			}
		}

		for (const FVector& Location : LeftAndRightSourceLocation)
		{
			if (RandomStream.FRandRange(0.0f, 1.0f) > PropsSpawnRate) { continue; }
			const FVector& Direction = CastForwardAndBack[0];
			FVector StartPoint = Location + Direction * SlotSize * 0.1;
			FVector EndPoint = StartPoint + Direction * SlotSize * 0.6;
				
			FHitResult HitResult;
			FCollisionQueryParams CollisionParams;
			CollisionParams.bTraceComplex = true;

			if (GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECC_WorldStatic, CollisionParams))
			{
				// 成功检测到墙面，处理该墙面
				// 随机偏移的方向, 是墙面的切线
				const FVector RandomMoveDirection = FVector::CrossProduct(HitResult.ImpactNormal, FVector::UpVector).GetSafeNormal();
				const float Offset = RandomStream.FRandRange(-0.01 * SlotSize, 0.01 * SlotSize);
				const FVector SpawnLocation = HitResult.Location + RandomMoveDirection * Offset - Direction * 80;
			
				FRotator SpawnRotation = FRotationMatrix::MakeFromX(HitResult.ImpactNormal).Rotator();
				SpawnRotation.Yaw -= 90.f;
			
				TSubclassOf<AActor> PropsClass = SpawnSmallPropsClasses[RandomStream.FRandRange(0, SpawnSmallPropsClasses.Num() - 1)];
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.Owner = this;
				AActor* Prop = GetWorld()->SpawnActor<AActor>(PropsClass, SpawnLocation, SpawnRotation, SpawnParameters);
				Prop->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			}
		}
	}
}

void AGeneratedMap::SpawnEnemy(int32 SlotIndex)
{
	if (RandomStream.FRandRange(0.f, 1.f) > EnemySpawnAccurate) { return; }

	const int32 SpawnCountIndex = Slots[SlotIndex]->MapCoordinate.Z % 2 == 0 ? Slots[SlotIndex]->MapCoordinate.Y : RealMapSize.Y - 1 - Slots[SlotIndex]->MapCoordinate.Y;
	const int32 SpawnNum = SpawnCount[SpawnCountIndex];
	FVector SpawnCenter = Slots[SlotIndex]->GetActorLocation() + FVector(0, 0, 50);
	UWorld* World = GetWorld();
	if (World && Enemies.Num() > 0)
	{
		for (int32 i = 0; i < SpawnNum; i++)
		{
			if (UNavigationSystemV1* NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World))
			{
				FVector SpawnLocation;
				FVector RandomSpawnOffset = FVector(RandomStream.FRandRange(-SlotSize * 0.4, SlotSize * 0.4), RandomStream.FRandRange(-SlotSize * 0.4, SlotSize * 0.4), 0.f);
				FNavLocation ProjectedLocation;
				SpawnLocation = SpawnCenter + RandomSpawnOffset;
				if (NavigationSystem->ProjectPointToNavigation(SpawnLocation, ProjectedLocation, FVector(100.f, 100.f, 300.f)))
				{
					if (FMath::Abs(ProjectedLocation.Location.Z - SpawnCenter.Z) < SlotHeight * 0.4)
					{
						SpawnLocation = ProjectedLocation.Location;
						UE_LOG(LogOpen, Display, TEXT("[ SpawnEnemy ] Source Slot is [%d, %d, %d] Get the ProjectLocation at location (%f, %f, %f)"), Slots[SlotIndex]->MapCoordinate.X, Slots[SlotIndex]->MapCoordinate.Y, Slots[SlotIndex]->MapCoordinate.Z, SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z);
					}
					FHitResult HitOnGround;
					GetWorld()->LineTraceSingleByChannel(HitOnGround, SpawnLocation + FVector(0.f, 0.f, 100.f), SpawnLocation - FVector(0.f, 0.f, 100.f), ECC_Visibility);
					SpawnLocation = HitOnGround.ImpactPoint + FVector(0,0,100.f);
					UE_LOG(LogOpen, Display, TEXT("[ SpawnEnemy ] Source Slot is [%d, %d, %d] Spawn Enemy at location (%f, %f, %f)"), Slots[SlotIndex]->MapCoordinate.X, Slots[SlotIndex]->MapCoordinate.Y, Slots[SlotIndex]->MapCoordinate.Z, SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z);
				}
				
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.Owner = this;
				SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
				AEnemy* NewEnemy = World->SpawnActor<AEnemy>(Enemies[RandomStream.RandRange(0, Enemies.Num() - 1)], SpawnLocation, FRotator::ZeroRotator, SpawnParameters);

				if (NewEnemy)
				{
					const int32 Level = Slots[SlotIndex]->MapCoordinate.Z + 1;
					NewEnemy->SetEnemyLevel(Level);
					NewEnemy->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
				}
			}
		}
	}
}

void AGeneratedMap::SpawnWeapon(int32 SlotIndex, int32 WeaponIndex, bool bForceSpawn)
{
	// 如果不要求强制生成
	if (SpawnWeaponClasses.Num() == 0) { return; }
	if (!bForceSpawn && RandomStream.FRandRange(0.f, 1.f) > WeaponSpawnAccurate) { return; }
	FVector SpawnCenter = Slots[SlotIndex]->GetActorLocation() + FVector(0, 0, 50);
	UWorld* World = GetWorld();

	FVector SpawnLocation = SpawnCenter + FVector(RandomStream.FRandRange(-50.f, 50.f), RandomStream.FRandRange(-50.f, 50.f), 0);
	if (WeaponIndex > 0)
	{
		WeaponIndex = SpawnWeaponClasses.Num() - 1;
		bHasWeaponGenerated = true;
	}
	TSubclassOf<AWeapon> WeaponClass = SpawnWeaponClasses[WeaponIndex];
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, SpawnLocation, FRotator::ZeroRotator, SpawnParameters);
	Weapon->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
}

void AGeneratedMap::SpawnGameKey(int32 SlotIndex, bool bForceSpawn)
{
	// 如果不要求强制生成
	if (!bForceSpawn && RandomStream.FRandRange(0.f, 1.f) > GameKeySpawnAccurate) { return; }
	FVector SpawnCenter = Slots[SlotIndex]->GetActorLocation() + FVector(0, 0, 50);
	UWorld* World = GetWorld();
	FVector SpawnLocation = SpawnCenter;

	if (World && GameKeyClass)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		AGameKey* GameKey = World->SpawnActor<AGameKey>(GameKeyClass, SpawnLocation, FRotator::ZeroRotator, SpawnParameters);
		GameKey->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	}
	bHasKeyGenerated = true;
}

void AGeneratedMap::SpawnEndDoor(int32 SlotIndex)
{
	FVector SpawnCenter = Slots[SlotIndex]->GetActorLocation() + FVector(0, 0, SlotHeight + 50.f);
	UWorld* World = GetWorld();
	FVector SpawnLocation = SpawnCenter;

	if (World && EndDoorClass)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		AEndDoor* EndDoor = World->SpawnActor<AEndDoor>(EndDoorClass, SpawnLocation, FRotator::ZeroRotator, SpawnParameters);
		if (EndDoor)
		{
			EndDoor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		}
	}
}

TArray<FIntVector2> AGeneratedMap::GetRandomRoadPointArray(const int32 Z)
{
	
	TArray<FIntVector2> RandomRoadPoints;
	TSet<FIntVector2> VisitedPoints;
	if (Z < 0) { return RandomRoadPoints; }
	
	FIntVector2 StartPoint = FIntVector2::ZeroValue;
	FIntVector2 EndPoint = EndPointArray[Z];
	if (Z > 0)
	{
		StartPoint = EndPointArray[Z - 1];
	}
	RandomRoadPoints.Add(StartPoint);
	VisitedPoints.Add(StartPoint);
	FIntVector2 CurrentPoint = StartPoint;
	TArray<FIntVector2> GoodRoadPoints;
	TArray<FIntVector2> BadRoadPoints;
	TArray<FIntVector2> NextRoadPoints;
	NextRoadPoints.SetNum(4);
	
	while (CurrentPoint != EndPoint)
	{
		GoodRoadPoints.Empty();
		BadRoadPoints.Empty();
		// 获取路径中最后一个元素的值
		if(RandomRoadPoints.IsEmpty())
		{
			UE_LOG(LogOpen, Warning, TEXT("Can't find a path from start to end"));
			break;
		}
		CurrentPoint = RandomRoadPoints.Last();

		if (CurrentPoint == EndPoint)
		{
			break;
		}

		int32 Distance = abs(EndPoint.X - CurrentPoint.X) + abs(EndPoint.Y - CurrentPoint.Y);
		
		// 获取可能的移动目标点
		NextRoadPoints[0] = CurrentPoint + FIntVector2(-1, 0);
		NextRoadPoints[1] = CurrentPoint + FIntVector2(1, 0);
		NextRoadPoints[2] = CurrentPoint + FIntVector2(0, -1);
		NextRoadPoints[3] = CurrentPoint + FIntVector2(0, 1);
		
		// 校验目标点是否有效 ( 是否在边界内, 是否未访问过 ), 将有效的点根据离终点的距离放入 GoodRoadPoints 或 BadRoadPoints
		for(int32 i = 0; i < NextRoadPoints.Num(); i++)
		{
			if(NextRoadPoints[i].X < 0 || NextRoadPoints[i].X > MapSize.X - 1 || NextRoadPoints[i].Y < 0 || NextRoadPoints[i].Y > MapSize.Y - 1)
			{	
				continue;
			}
			if(VisitedPoints.Contains(NextRoadPoints[i]))
			{
				continue;
			}
			int32 NewDistance = abs(EndPoint.X - NextRoadPoints[i].X) + abs(EndPoint.Y - NextRoadPoints[i].Y);
			if(NewDistance < Distance)
			{
				GoodRoadPoints.Add(NextRoadPoints[i]);
			}
			else
			{
				BadRoadPoints.Add(NextRoadPoints[i]);
			}
		}

		bool bCanMove = false;

		float RandomNum = RandomStream.FRandRange(0.0f, 1.0f);
		if (!GoodRoadPoints.IsEmpty() && (RandomNum < AccurateOfStraightRoad || BadRoadPoints.IsEmpty()))
		{
			const int32 RandomIndex = RandomStream.RandRange(0, GoodRoadPoints.Num() - 1);
			RandomRoadPoints.Add(GoodRoadPoints[RandomIndex]);
			VisitedPoints.Add(GoodRoadPoints[RandomIndex]);
			bCanMove = true;
		}
		else if (!BadRoadPoints.IsEmpty())
		{
			const int32 RandomIndex = RandomStream.RandRange(0, BadRoadPoints.Num() - 1);
			RandomRoadPoints.Add(BadRoadPoints[RandomIndex]);
			VisitedPoints.Add(BadRoadPoints[RandomIndex]);
			bCanMove = true;
		}
		
		if (!bCanMove)
		{
			UE_LOG(LogOpen, Warning, TEXT("Backtracking from (%d, %d)"), CurrentPoint.X, CurrentPoint.Y);
			RandomRoadPoints.Pop();
		}
	}
	if (!RandomRoadPoints.IsEmpty() && RandomRoadPoints.Last() != EndPoint )
	{
		RandomRoadPoints.Add(EndPoint);
	}
	
	return RandomRoadPoints;
}



