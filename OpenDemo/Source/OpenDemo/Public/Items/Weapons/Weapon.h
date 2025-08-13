#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"

#include "Items/Item.h"
#include "Items/ItemTypes.h"
#include "Weapon.generated.h"

class USoundBase;
class UBoxComponent;

UCLASS()
class OPENDEMO_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	AWeapon();

	void EquipWeapon(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstig);
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);
	void SetVisibility(bool bCanSee);

	FName GetWeaponSocketName();

	TArray<TObjectPtr<AActor>> IgnoredActors;
protected:
	// from AActor
	virtual void BeginPlay() override;

	// from AItem
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	
	UFUNCTION()
	void OnHitBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnHitBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

private:
	void BoxTrace(FHitResult& BoxHit);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TObjectPtr<USoundBase> EquipSound;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TObjectPtr<UBoxComponent> HitBox;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<USceneComponent> BoxTraceStart;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<USceneComponent> BoxTraceEnd;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	EWeaponType WeaponType = EWeaponType::EWT_Body;

	// debug settings
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceExtent = FVector(5.f, 5.f, 5.f);
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowDebugBoxTrace = false;

public:
	FORCEINLINE UBoxComponent* GetHitBox() const { return HitBox; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }

};