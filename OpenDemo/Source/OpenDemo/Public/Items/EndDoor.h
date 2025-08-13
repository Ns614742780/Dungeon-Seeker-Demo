#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "EndDoor.generated.h"


UCLASS()
class OPENDEMO_API AEndDoor : public AItem
{
	GENERATED_BODY()

public:
	AEndDoor();	

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UWidgetComponent> ForbidWidgetComponent;
	
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
