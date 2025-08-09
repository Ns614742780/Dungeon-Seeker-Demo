// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/EndDoor.h"

#include "Components/WidgetComponent.h"
#include "Interfaces/PickupInterface.h"

AEndDoor::AEndDoor()
{
	ForbidWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ForbidWidgetComponent"));
	ForbidWidgetComponent->SetupAttachment(RootComponent);
	ForbidWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	ForbidWidgetComponent->SetVisibility(false);
}

void AEndDoor::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		if (PickupInterface->HasKey())
		{
			Super::OnSphereBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
		}
		else
		{
			PickupInterface->SetOverlappingItem(this);
			ForbidWidgetComponent->SetVisibility(true);
		}
	}
	
}

void AEndDoor::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		if (PickupInterface->HasKey())
		{
			Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
		}
		else
		{
			PickupInterface->SetOverlappingItem(this);
			ForbidWidgetComponent->SetVisibility(false);
		}
	}
}
