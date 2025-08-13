#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// 定义 gameplay tag 以便在 C++ 与蓝图中均可使用

struct FOpenGameplayTags
{
public:
	static const FOpenGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();

	// map
	FGameplayTag Map_ConnectID_00;
	FGameplayTag Map_ConnectID_01;
	FGameplayTag Map_ConnectID_02;
	FGameplayTag Map_ConnectID_03;
	FGameplayTag Map_ConnectID_04;
	FGameplayTag Map_ConnectID_05;
	FGameplayTag Map_ConnectID_06;
	FGameplayTag Map_ConnectID_07;
	FGameplayTag Map_ConnectID_08;
	FGameplayTag Map_ConnectID_09;
	FGameplayTag Map_ConnectID_10;

	// attribute
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;

	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxPower;

	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_ArmorBlockChance;

	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_PowerRegeneration;

	FGameplayTag Attributes_Resistance_All;
	
	FGameplayTag Attributes_Resistance_Fire;
	FGameplayTag Attributes_Resistance_Lightning;
	FGameplayTag Attributes_Resistance_Arcane;
	FGameplayTag Attributes_Resistance_Physical;

	FGameplayTag Attributes_Meta_IncomingXP;


	// input
	FGameplayTag Input_LeftMouseButton;
	FGameplayTag Input_RightMouseButton;
	FGameplayTag Input_MouseX;
	FGameplayTag Input_MouseY;

	FGameplayTag Input_LeftShift;
	FGameplayTag Input_LeftAlt;
	FGameplayTag Input_Space;

	FGameplayTag Input_KeyE;
	FGameplayTag Input_KeyQ;
	FGameplayTag Input_KeyR;
	FGameplayTag Input_KeyF;

	FGameplayTag Input_1;
	FGameplayTag Input_2;
	FGameplayTag Input_3;
	FGameplayTag Input_4;


	// damage
	FGameplayTag Damage;
	
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Lightning;
	FGameplayTag Damage_Arcane;
	FGameplayTag Damage_Physical;

	FGameplayTag Damage_True;
	
	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;

	// ability
	FGameplayTag Ability_HitReact;
	FGameplayTag Ability_Attack;
	FGameplayTag Ability_Summon;

	FGameplayTag Ability_Fire_FireBall;

	// cooldown
	FGameplayTag Cooldown_Fire_FireBall;
	FGameplayTag Cooldown_Summon;

	// event
	FGameplayTag Event_Weapon_Hit;
	FGameplayTag Event_GetHit;

	
private:
	static FOpenGameplayTags GameplayTags;

};