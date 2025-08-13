// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Items/OpenEffectActor.h"

// Sets default values
ABreakableActor::ABreakableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);

	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(RootComponent);

	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

// Called when the game starts or when spawned
void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImapctPoint, AActor* Hitter)
{
}

bool ABreakableActor::IsItem_Implementation()
{
	return true;
}

void ABreakableActor::SpawnPotion()
{
	// 确保只生成一次
	if (bIsBroken) { return; }
	UWorld* World = GetWorld();
	if (World && PotionClasses.Num() > 0)
	{
		FVector Location = GetActorLocation();
		Location.Z += 75.f;

		uint32 RandomIndex = FMath::RandRange(0, PotionClasses.Num() - 1);

		World->SpawnActor<AOpenEffectActor>(PotionClasses[RandomIndex], Location, GetActorRotation());
		bIsBroken = true;
	}
}
