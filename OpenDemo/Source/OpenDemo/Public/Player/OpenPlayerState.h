#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"

#include "Characters/CharacterTypes.h"

#include "OpenPlayerState.generated.h"

class ULevelUpInfo;
class UAbilitySystemComponent;
class UAttributeSet;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, int32 /* StateValue */);
DECLARE_MULTICAST_DELEGATE()

UCLASS()
class OPENDEMO_API AOpenPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AOpenPlayerState();

	// from APlayerState
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// form Ability System
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const;

	// self
	void AddToXP(int32 InXP);
	void AddToLevel(int32 InLevel);
	void AddToAttributePoints(int32 InPoints);
	void AddToSpellPoints(int32 InPoints);
	
	void SetXP(int32 InXP);
	void SetLevel(int32 InLevel);

	FOnPlayerStateChanged OnPlayerXPChangedDelegate;
	FOnPlayerStateChanged OnPlayerLevelChangedDelegate;
	FOnPlayerStateChanged OnAttributePointChangedDelegate;
	FOnPlayerStateChanged OnSpellPointChangedDelegate;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;

protected:

	// 在多人游戏中玩家的能力系统与属性集交给 PalyerState 管理
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

private:
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 PlayerLevel = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 PlayerXP = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_AttributePoint)
	int32 AttributePoint = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SpellPoint)
	int32 SpellPoint = 1;


	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UFUNCTION()
	void OnRep_XP(int32 OldXP);

	UFUNCTION()
	void OnRep_AttributePoint(int32 OldAttributePoint);

	UFUNCTION()
	void OnRep_SpellPoint(int32 OldSpellPoint);

public:
	FORCEINLINE int32 GetPlayerLevel() const { return PlayerLevel; }
	FORCEINLINE int32 GetPlayerXP() const { return PlayerXP; }
	FORCEINLINE int32 GetAttributePoint() const { return AttributePoint; }
	FORCEINLINE int32 GetSpellPoint() const { return SpellPoint; }
	
};
