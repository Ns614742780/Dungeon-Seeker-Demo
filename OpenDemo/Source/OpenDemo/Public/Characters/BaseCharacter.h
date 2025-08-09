#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interfaces/CombatInterface.h"

#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"

#include "BaseCharacter.generated.h"

class AWeapon;
class UAnimMontage;
class UGameplayEffect;
class UGameplayAbility;
class UAttributeComponent;
class UAbilitySystemComponent;
class UAttributeSet;

UCLASS(Abstract)
class OPENDEMO_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface, public ICombatInterface,public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	// from AActor
	virtual void Tick(float DeltaTime) override;
	
	// for ability system
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const;

protected:
	// from AActor
	virtual void BeginPlay() override;

	// from IHitInterface
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual bool IsItem_Implementation() override;
	
	// from ICombatInterface
	virtual void Die() override;
	virtual FVector GetCombatSocketLocation() override;
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual FName GetHitReactSectionName_Implementation(FVector HitPoint) override;
	virtual UAnimMontage* GetAttackMontage_Implementation() override;
	virtual int32 GetAttackSectionCount_Implementation() override;
	virtual int32 GetMinionCount_Implementation() override;
	virtual void AddMinionCount_Implementation(int32 Amount) override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual ECharacterClass GetCharacterClass_Implementation() override;
	
	// self
	virtual void InitAbilityActorInfo();
	void ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& GameplayEffectClass, const float Level) const;
	virtual void InitializeDefaultAttributes() const;	// 初始化角色的属性
	void AddCharacterAbilities();	// 将能力添加到角色

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();	// 攻击结束
	
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);
	
	void DisableCapsuleCollision();
	void DisableMeshCollision();
	
	UFUNCTION(NetMulticast, reliable)
	virtual void MulticastHandleDeath();


	// components
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> MeshComponent;	// 用于访问自身的mesh,以供绑定 socket 使用

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;		// 能力系统组件
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;							// 能力系统的属性集

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;		// 用于初始化主要属性
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;	// 用于初始化次要属性

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultMainAttributes;			// 用于初始化主属性

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Paladin;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName SpellSocketName = "SpellSocket";	// 用于释放法术的 socket
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;	// 当前交战的对象,用于 enemy 与 player

	int32 MinionsCount = 0;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon")
	TArray<TObjectPtr<AWeapon>> Weapons;	// 武器列表
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon")
	int ActiveWeaponIndex = 0;		// 当前使用的武器
	int WeaponCount = 0;			// 武器总数

	// play montage
	virtual int32 PlayDeathMontage();
	
	UPROPERTY(EditAnywhere, Category = Combat)
	double WarpTargetDistance = 75.f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float WalkSpeed = 200.f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float RunSpeed = 600.f;
	UPROPERTY(BlueprintReadOnly, Category = Montages)
	TEnumAsByte<EDeathPose> DeathPose;

	virtual bool CanAttack() const;
	virtual void Attack();
	bool IsAlive() const;
	
	void GetWeapon(AWeapon* Weapon);
	void DirectionalHitReact(const FVector& ImpactPoint);
	void PlayHitSound(const FVector& ImpactPoint);
	void PlayHitParticles(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);

	/*
		play montage functions
	*/
	void PlayMontageSection(UAnimMontage* AnimMontage, const FName& SectionName);
	void PlayHitReactMontage(const FName& SectionName);
	virtual void PlayAttackMontage();
	void StopAttackMontage();
	

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();
	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

private:

	bool bIsDead = false;

	// 用于初始化属性
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilities;

	// Animation Montages
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> HitReactMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> DeathMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> AttackMontage;
	
	UPROPERTY(EditAnywhere, Category = Montages)
	TArray<FName> AttackMontageSections;
	UPROPERTY(EditAnywhere, Category = Montages)
	TArray<FName> DeathMontageSections;

	// Sounds and Effects
	UPROPERTY(EditAnywhere, Category = Combat)
	USoundBase* HitSound;
	UPROPERTY(EditAnywhere, Category = Combat)
	UParticleSystem* HitParticles;


public:
	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }
	FORCEINLINE float GetWalkSpeed() const { return WalkSpeed; }
	FORCEINLINE float GetRunSpeed() const { return RunSpeed; }

};
