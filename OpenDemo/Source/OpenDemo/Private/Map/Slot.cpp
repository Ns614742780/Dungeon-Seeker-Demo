#include "Map/Slot.h"

#include "Map/ModuleSet.h"
#include "OpenDemo/OpenLogChannels.h"


ASlot::ASlot()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	RootComponent = StaticMeshComponent;
}

void ASlot::InitRandomStream(int32 Seed)
{
	RandomStream.Initialize(Seed);
}

void ASlot::CalculateEntropy()
{
	Entropy = 0.f;
	float TotalWeight = 0.f;
	for (const FOpenModuleState ModuleState : MaybeModuleSet->MaybeModules)
	{
		TotalWeight += ModuleState.Weight;
	}
	
	for (const FOpenModuleState ModuleState : MaybeModuleSet->MaybeModules)
	{
		float Probability = ModuleState.Weight / TotalWeight;
		Entropy -= Probability * FMath::Log2(Probability);
	}
	UE_LOG(LogOpen, Display, TEXT("[ Slot ] Location (%d, %d, %d) The %s New Entropy is %f"), MapCoordinate.X, MapCoordinate.Y, MapCoordinate.Z, *GetName(),Entropy);
}

void ASlot::Collapse()
{
	float TotalWeight = 0.f;
	for (const FOpenModuleState ModuleState : MaybeModuleSet->MaybeModules)
	{
		TotalWeight += ModuleState.Weight;
	}
	if (MaybeModuleSet->MaybeModules.Num() > 1)
	{
		float RandomValue = RandomStream.FRandRange(0.f, TotalWeight);
		float SelectWeight = 0.f;
		for (const FOpenModuleState ModuleState : MaybeModuleSet->MaybeModules)
		{
			SelectWeight += ModuleState.Weight;
			if (SelectWeight >= RandomValue)
			{
				CollapseState = ModuleState;
				bIsCollapsed = true;
				MaybeModuleSet->Empty();
				TSet<FOpenModuleState> AddModules;
				AddModules.Add(CollapseState);
				MaybeModuleSet->AddModule(AddModules);

				if (CollapseState.Mesh != nullptr)
				{
					StaticMeshComponent->SetStaticMesh(CollapseState.Mesh);
                    StaticMeshComponent->SetRelativeRotation(FRotator(0.f, CollapseState.Rotation * 90.f, 0.f));
                    StaticMeshComponent->SetRelativeScale3D(GetOwner()->GetActorScale3D());
				}
				
				break;
			}
		}
	}
	else if (MaybeModuleSet->MaybeModules.Num() == 1)
	{
		CollapseState = MaybeModuleSet->MaybeModules.Array()[0];
		bIsCollapsed = true;
		MaybeModuleSet->Empty();

		TSet<FOpenModuleState> AddModules;
		AddModules.Add(CollapseState);
		MaybeModuleSet->AddModule(AddModules);
		
		if (CollapseState.Mesh != nullptr)
		{
			StaticMeshComponent->SetStaticMesh(CollapseState.Mesh);
            StaticMeshComponent->SetRelativeRotation(FRotator(0.f, CollapseState.Rotation * 90.f, 0.f));
            StaticMeshComponent->SetRelativeScale3D(GetOwner()->GetActorScale3D());
		}
	}
	else
	{
		bIsCollapsed = true;
		UE_LOG(LogOpen, Display, TEXT("[ ASlot ] Has no module can select"));
	}
	if (CollapseState.Mesh != nullptr)
	{
		UE_LOG(LogOpen, Display, TEXT("[ ASlot ] Choose mesh %s"), *CollapseState.Mesh->GetName());
	}
}

void ASlot::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World && ModuleSetClass)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		MaybeModuleSet = World->SpawnActor<AModuleSet>(ModuleSetClass, SpawnParameters);
		MaybeModuleSet->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	}
	
}

void ASlot::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
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

