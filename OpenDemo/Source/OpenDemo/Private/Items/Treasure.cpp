// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
#include "Characters/OpenCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

void ATreasure::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AOpenCharacter* OpenCharacter = Cast<AOpenCharacter>(OtherActor); // 设置当前物品
	if (OpenCharacter)
	{
		if (PickUpSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PickUpSound, GetActorLocation());
		}
		Destroy();
	}
}
