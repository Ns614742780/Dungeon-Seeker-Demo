#pragma once

// 
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_HasWeaponUnequipped UMETA(DisplayName = "HasWeaponUnequipped"),
	ECS_EquippedLeftHand UMETA(DisplayName = "EquippedLeftHand"),
	ECS_EquippedRightHand UMETA(DisplayName = "EquippedRightHand"),
	ECS_EquippedBothHands UMETA(DisplayName = "EquippedBothHand")
};

// ��ɫ����״̬
UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_HitReaction UMETA(DisplayName = "HitReaction"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_Interacting UMETA(DisplayName = "Interacting"),
	EAS_Equipping UMETA(DisplayName = "Equipping"),
	EAS_Dodge UMETA(DisplayName = "Dodge"),
	EAS_Dead UMETA(DisplayName = "Dead")
};

// 
UENUM(BlueprintType)
enum EDeathPose
{
	EDP_Death1 UMETA(DisplayName = "Death1"),
	EDP_Death2 UMETA(DisplayName = "Death2"),
	EDP_Death3 UMETA(DisplayName = "Death3"),
	EDP_Death4 UMETA(DisplayName = "Death4"),

	EDP_MAX UMETA(Hidden, DisplayName = "Max")
};

// 
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_Engaged UMETA(DisplayName = "Engaged"),

	EES_NULL UMETA(Hidden, DisplayName = "Null"),
};
