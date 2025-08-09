#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Items/Weapons/Weapon.h"
#include "Interfaces/PickupInterface.h"
#include "Interfaces/PlayerInterface.h"

#include "OpenCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UOpenUserWidget;
class AItem;

UCLASS()
class OPENDEMO_API AOpenCharacter : public ABaseCharacter, public IPickupInterface, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AOpenCharacter();

	// from AActor
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Jump() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// from ACharacter
	virtual void PossessedBy(AController* NewController) override;

	// from APawn
	virtual void OnRep_PlayerState() override;
	
	// from IPickupInterface
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual bool HasKey() override;
	virtual void GetKey() override;

	// from IPlayerInterface
	virtual int32 GetXP_Implementation() const override;
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual int32 FindLevelForXP_Implementation(int32 InXP) const override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetAttributePoint_Implementation() const override;
	virtual int32 GetSpellPoint_Implementation() const override;
	virtual void AddToPlayerLevel_Implementation(int32 InPlayerLevel) override;
	virtual int32 GetAttributePointReward_Implementation(int32 InLevel) const override;
	virtual int32 GetSpellPointReward_Implementation(int32 InLevel) const override;
	virtual void AddToAttributePoints_Implementation(int32 InAttributePointsNum) override;
	virtual void AddToSpellPoints_Implementation(int32 InSpellPointsNum) override;
	
	// self
	UFUNCTION(BlueprintCallable)
	UAnimMontage* GetSpellMontage();

	UFUNCTION(BlueprintCallable)
	UAnimMontage* GetRitualMontage();

	UFUNCTION(BlueprintCallable)
	FName GetCharacterStateSection();

	UFUNCTION(BlueprintCallable)
	bool CanMeleeAttack();

	void HandleEKey();
	void HandleQKey();

protected:
	// from AActor
	virtual void BeginPlay() override;

	// from ABaseCharacter
	virtual void InitAbilityActorInfo() override;

	// from ICombatInterface
	virtual int32 GetPlayerLevel_Implementation() override;
	virtual void Die() override;
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	
	virtual void Attack() override;
	virtual bool CanAttack() const override;

	void PlayEquipMontage();
	virtual void AttackEnd() override;
	
	UFUNCTION(BlueprintCallable)
	void EquipEnd();
	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	UFUNCTION(BlueprintCallable)
	void DisArm();
	UFUNCTION(BlueprintCallable)
	void Equip();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UParticleSystemComponent> LevelUpCascadeComponent;

private:
	
	// self
	void SetCharacterStateByActiveWeapon();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpNiagara() const;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> ViewCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> SpellMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> RitualMontage;

	UPROPERTY(EditDefaultsOnly, Category = "BGM")
	TObjectPtr<UAudioComponent> BGMComponent;

	UPROPERTY(EditAnywhere, Category = "BGM")
	TObjectPtr<USoundBase> BGMSound;
	
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;
	
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;
	UPROPERTY()
	UOpenUserWidget* OpenOverlay;
	
	// motages
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;

	bool bHasKey = false;

public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }

};
