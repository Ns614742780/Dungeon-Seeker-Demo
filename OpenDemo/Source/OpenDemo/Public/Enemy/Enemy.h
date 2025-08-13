// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"

#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interfaces/EnemyInterface.h"

#include "Enemy.generated.h"

class AAIController;
class UPawnSensingComponent;
class UWidgetComponent;
class UBehaviorTree;
class AOpenAIController;

UCLASS()
class OPENDEMO_API AEnemy : public ABaseCharacter, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AEnemy();

	// from ACharacter
	virtual void PossessedBy(AController* NewController) override;
	
	// from AActor
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;  // Called when the enemy is destroyed

	// from IHitInterface
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	// from ICombatInterface
	virtual int32 GetPlayerLevel_Implementation() override;
	virtual void Die() override;
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;

	// from IEnemyInterface
	virtual void SetCombatSpeed_Implementation() override;
	virtual void SetPatrolSpeed_Implementation() override;

	// self
	UFUNCTION(BlueprintCallable)
	void SetEnemyLevel(int32 InLevel);
	
	// from UOverlayWidgetController
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting = false;
	
protected:
	// from AActor
	virtual void BeginPlay() override;
	
	// from ABaseCharacter
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;

	// self
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	void HandleLevelSet();
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Enemy Properties")
	EEnemyState EnemyState = EEnemyState::EES_Patrolling; // Current state of the enemy
	// properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 CharacterLevel = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AOpenAIController> OpenAIController;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<UPawnSensingComponent> PawnSensing;

	UPROPERTY(EditAnywhere, Category = "AI")
	double AttackRadius = 150.f; // Distance to consider the target in attack range

private:

	// components
	
	UPROPERTY(EditAnywhere, Category = Weapons)
	TSubclassOf<AWeapon> WeaponClass; // Class of the weapon the enemy can use

	// combat properties
	UPROPERTY(EditAnywhere, Category = Combat)
	double CombatRadius = 1000.f; // Distance to consider the target in combat
	UPROPERTY(EditAnywhere, Category = Combat)
	bool bIsPlayerFriend = false;
	

	FTimerHandle AttackTimer; // Timer handle for combat behavior
	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMinTime = 0.5f; // Minimum wait time at each patrol point
	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMaxTime = 1.5f; // Maximum wait time at each patrol point

	// navigation properties
	UPROPERTY()
	TObjectPtr<AAIController> EnemyController; // Reference to the AI controller managing this enemy

	// patrol properties
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets; // Array of patrol points for the enemy to navigate through
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget; // Target for the enemy to patrol towards
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double PatrolRadius = 200.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double AcceptanceRadius = 50.f;	// The Radius within which the enemy considers it has reached the patrol target

	FTimerHandle PatrolTimer; // Timer handle for patrol behavior
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMinTime = 2.f; // Minimum wait time at each patrol point
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMaxTime = 5.f; // Maximum wait time at each patrol point

	UPROPERTY(EditAnywhere, Category = "Enemy Properties")
	float DeathLifeSpan = 5.0f; // Time before the enemy is destroyed after death

	// patrol function
	void PatrolTimerFinished();
	void ClearPatrolTimer();

	// AI behavior functions
	void CheckPatrolTarget();
	void LoseInterest();
	void StartPatrolling();
	bool IsOutSideCombatRadius() const;
	bool IsOutSideAttackRadius() const;
	bool IsInSideAttackRadius() const;
	bool IsChasing() const;
	bool IsAttacking() const;
	bool IsEngaged() const;

	bool InTargetRange(AActor* Target, double Radius) const;
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();
	void SpawnDefaultWeapon();

};
