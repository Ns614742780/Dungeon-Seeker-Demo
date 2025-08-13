#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"

#include "OpenPlayerController.generated.h"

class UDamageTextComponent;
class UInputMappingContext;
class UInputAction;

class UOpenInputConfig;
class UOpenAbilitySystemComponent;

struct FInputActionValue;

UCLASS()
class OPENDEMO_API AOpenPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AOpenPlayerController();

	// self
	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, const bool bBlockedHit, const bool bCriticalHit);

protected:
	// from AActor
	virtual void BeginPlay() override;
	
	// from APlayerController
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> OpenInputMappingContext;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> TurnAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookUpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ShiftAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> AltAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractionAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> WeaponChange;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UOpenInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UOpenAbilitySystemComponent> OpenAbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	float WalkSpeed = 300.f;
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	float RunSpeed = 600.0f;

	float FollowTime = 0.0f;
	float ShortPressThreshold = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
	
	UOpenAbilitySystemComponent* GetASC();
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	void Move(const FInputActionValue& InputActionValue);

	void Turn(const FInputActionValue& InputActionValue);
	void LookUp(const FInputActionValue& InputActionValue);
	
	void ShiftPressed();
	void ShiftReleased();
	void AltPressed();
	void AltReleased();

	void EKeyPressed();
	void QKeyPressed();

public:
	FORCEINLINE void SetWalkSpeed(float Speed) { WalkSpeed = Speed; }
	FORCEINLINE void SetRunSpeed(float Speed) { RunSpeed = Speed; }
};
