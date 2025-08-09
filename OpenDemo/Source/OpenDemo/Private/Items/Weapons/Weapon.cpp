#include "Items/Weapons/Weapon.h"

#include "Characters/OpenCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"
#include "OpenGameplayTags.h"
#include "AbilitySystem/OpenAbilitySystemLibrary.h"
#include "Components/WidgetComponent.h"
#include "OpenDemo/OpenLogChannels.h"
#include "Styling/ToolBarStyle.h"

AWeapon::AWeapon()
{	
	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox")); // Create a box component for hit detection
	HitBox->SetupAttachment(RootComponent); // Attach the HitBox to the root component
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // Enable collision for hit detection
	HitBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	HitBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceStart")); // Create a scene component for the start of the box trace
	BoxTraceStart->SetupAttachment(RootComponent); // Attach the BoxTraceStart to the root component

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceEnd")); // Create a scene component for the end of the box trace
	BoxTraceEnd->SetupAttachment(RootComponent); // Attach the BoxTraceEnd to the root component
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);
	HitBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnHitBoxBeginOverlap);
}

void AWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AWeapon::OnHitBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Ignore hits if the owner is an enemy and the other actor is an enemy
	if (UOpenAbilitySystemLibrary::IsFriend(GetOwner(), OtherActor)) { return; }
	
	FHitResult BoxHit;
	BoxTrace(BoxHit);

	if (HasAuthority())
	{
		if (AActor* HitActor = BoxHit.GetActor())
		{
			if (UOpenAbilitySystemLibrary::IsFriend(GetOwner(), HitActor)) { return; }
			// check if the actor is alive and active the weapon hit event
			UE_LOG(LogOpen, Warning, TEXT("[ Weapon ] Owner is %s"), *GetOwner()->GetName());
			
			if (const ABaseCharacter* Character = Cast<ABaseCharacter>(GetOwner()))
			{
				if (UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent())
				{
					FGameplayEventData EventData;
                    EventData.EventTag = FOpenGameplayTags::Get().Event_Weapon_Hit;
                    EventData.Target = HitActor;
                    ASC->HandleGameplayEvent(EventData.EventTag, &EventData);
				}
			}
			if (const IHitInterface* HitInterface = Cast<IHitInterface>(HitActor))
			{
				HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
			}
			IgnoredActors.AddUnique(BoxHit.GetActor());
			
			CreateFields(BoxHit.ImpactPoint);
		}
	}
}

void AWeapon::OnHitBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}


void AWeapon::EquipWeapon(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstig)
{
	SetOwner(NewOwner); // Set the owner of the weapon
	SetInstigator(NewInstig); // Set the instigator of the weapon

	AttachMeshToSocket(InParent, InSocketName);
	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Disable the sphere collision
	}
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this, EquipSound, GetActorLocation()); // Play the equip sound at the weapon's location
	}
	if (ItemEffect)
	{
		ItemEffect->Deactivate(); // Deactivate the ember effect if it exists
		ItemEffect->SetVisibility(false);
	}
	if (WidgetComponent)
	{
		WidgetComponent->SetVisibility(false);
	}
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::SetVisibility(bool bCanSee)
{
	if (!bCanSee)
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
	}
	else
	{
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
	}
}

FName AWeapon::GetWeaponSocketName()
{
	FName WeaponSocketName;
	switch (WeaponType)
	{
	case EWeaponType::EWT_LeftHanded:
		WeaponSocketName = FName("LeftHandSocket"); // Set the socket name for left-handed weapons
		break;
	case EWeaponType::EWT_RightHanded:
		WeaponSocketName = FName("RightHandSocket"); // Set the socket name for right-handed weapons
		break;
	case EWeaponType::EWT_TwoHanded:
		WeaponSocketName = FName("TwoHandedSocket"); // Set the socket name for two-handed weapons
		break;
	case EWeaponType::EWT_Body:
		WeaponSocketName = FName("BodySocket"); // Set the socket name for body weapons
		break;
	default:
		break;
	}

	return WeaponSocketName; // Return the socket name based on the weapon type
}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	const FVector Start = BoxTraceStart->GetComponentLocation(); // Get the start location of the box trace
	const FVector End = BoxTraceEnd->GetComponentLocation(); // Get the end location of the box trace

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this); // Ignore self in the trace
	ActorsToIgnore.Add(Owner); // Ignore the owner of the weapon in the trace

	for (AActor* IgnoredActor : IgnoredActors)
	{
		ActorsToIgnore.AddUnique(IgnoredActor); // Add previously hit actors to the ignore list
	}

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		BoxTraceExtent, // Box extent
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1, // Trace channel
		false, // Whether to ignore self
		ActorsToIgnore, // Actors to ignore
		bShowDebugBoxTrace ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		BoxHit, // Hit result
		true // Whether to return the hit result
	);
	IgnoredActors.AddUnique(BoxHit.GetActor());
}

