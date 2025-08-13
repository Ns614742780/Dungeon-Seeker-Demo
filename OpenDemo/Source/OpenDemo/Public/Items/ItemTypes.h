#pragma once

// 物品状态
enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Highlighted,
	EIS_Droped,
	EIS_Equipped,
	EIS_Interacting,
};

// 武器类型
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_LeftHanded UMETA(DisplayName = "One Handed"),
	EWT_RightHanded UMETA(DisplayName = "One Handed Right"),
	EWT_TwoHanded UMETA(DisplayName = "Two Handed"),
	EWT_Body UMETA(DisplayName = "Body"),

	EWT_NULL UMETA(Hidden, DisplayName = "NULL")
};