#include "OpenGameplayTags.h"

#include "GameplayTagsManager.h"

// 在 CPP 中定义 gameplay tags 以供 CPP 与蓝图共同使用
FOpenGameplayTags FOpenGameplayTags::GameplayTags;
void FOpenGameplayTags::InitializeNativeGameplayTags()
{
	// Map connector id
	GameplayTags.Map_ConnectID_00 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Map.ConnectID.00"));
	GameplayTags.Map_ConnectID_01 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Map.ConnectID.01"));
	GameplayTags.Map_ConnectID_02 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Map.ConnectID.02"));
	GameplayTags.Map_ConnectID_03 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Map.ConnectID.03"));
	GameplayTags.Map_ConnectID_04 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Map.ConnectID.04"));
	GameplayTags.Map_ConnectID_05 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Map.ConnectID.05"));
	GameplayTags.Map_ConnectID_06 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Map.ConnectID.06"));
	GameplayTags.Map_ConnectID_07 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Map.ConnectID.07"));
	GameplayTags.Map_ConnectID_08 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Map.ConnectID.08"));
	GameplayTags.Map_ConnectID_09 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Map.ConnectID.09"));
	GameplayTags.Map_ConnectID_06 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Map.ConnectID.10"));

	
	// Primary Attributes
	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"), 
		FString("Increases Physical Damage"));
	GameplayTags.Attributes_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Intelligence"), 
		FString("Increases Magical Damage"));
	GameplayTags.Attributes_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Resilience"), 
		FString("Increases Armor and Armor Penetration"));
	GameplayTags.Attributes_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Vigor"), 
		FString("Increases Health"));

	// Secondary Attributes
	GameplayTags.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxHealth"), 
		FString("Increases maximum health"));
	GameplayTags.Attributes_Secondary_MaxPower = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxPower"), 
		FString("Increases maximum power"));
	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.Armor"), 
		FString("Reduce damage taken, improves Block Chance"));
	GameplayTags.Attributes_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.ArmorPenetration"), 
		FString("Increases damage dealt by ignoring a portion of the target's armor"));
	GameplayTags.Attributes_Secondary_ArmorBlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.ArmorBlockChance"), 
		FString("Increases chance to block attacks"));
	GameplayTags.Attributes_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitChance"), 
		FString("Increases chance to deal critical hits"));
	GameplayTags.Attributes_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitDamage"), 
		FString("Increases damage dealt by critical hits"));
	GameplayTags.Attributes_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitResistance"), 
		FString("Reduces damage taken from critical hits"));
	GameplayTags.Attributes_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.HealthRegeneration"), 
		FString("Regenerates health every 1 second"));
	GameplayTags.Attributes_Secondary_PowerRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.PowerRegeneration"), 
		FString("Regenerates power every 1 second"));

	// Input Tags
	GameplayTags.Input_LeftMouseButton = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.LeftMouseButton"), 
		FString("Left mouse button input"));
	GameplayTags.Input_RightMouseButton = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.RightMouseButton"), 
		FString("Right mouse button input"));
	GameplayTags.Input_MouseX = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.MouseX"),
		FString("Move mouse in X axis"));
	GameplayTags.Input_MouseY = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.MouseY"),
		FString("Move mouse in Y axis"));

	GameplayTags.Input_LeftShift = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.LeftShift"), 
		FString("Left Shift key input"));
	GameplayTags.Input_LeftAlt = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.LeftAlt"), 
		FString("Left Alt key input"));
	GameplayTags.Input_Space = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.Space"), 
		FString("Space key input"));

	GameplayTags.Input_KeyE = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.KeyE"), 
		FString("E key input"));
	GameplayTags.Input_KeyQ = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.KeyQ"), 
		FString("Q key input"));
	GameplayTags.Input_KeyR = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.KeyR"), 
		FString("R key input"));
	GameplayTags.Input_KeyF = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.KeyF"), 
		FString("F key input"));

	GameplayTags.Input_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.1"), 
		FString("Input 1 key"));
	GameplayTags.Input_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.2"), 
		FString("Input 2 key"));
	GameplayTags.Input_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.3"), 
		FString("Input 3 key"));
	GameplayTags.Input_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.4"), 
		FString("Input 4 key"));

	// Damage
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage"),
		FString("Damage"));
	GameplayTags.Damage_True = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage_True"),
		FString("Damage True"));
	GameplayTags.Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Fire"),
		FString("Fire Damage Type"));
	GameplayTags.Damage_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Lightning"),
		FString("Lightning Damage Type"));
	GameplayTags.Damage_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Arcane"),
		FString("Arcane Damage Type"));
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Physical"),
		FString("Physical Damage Type"));

	// Damage Resistances
	GameplayTags.Attributes_Resistance_All = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Resistance.All"),
		FString("Resistance All Damage Type"));
	GameplayTags.Attributes_Resistance_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Resistance.Fire"),
		FString("Resistance Fire Damage Type"));
	GameplayTags.Attributes_Resistance_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Resistance.Lightning"),
		FString("Resistance Lightning Damage Type"));
	GameplayTags.Attributes_Resistance_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Resistance.Arcane"),
		FString("Resistance Arcane Damage Type"));
	GameplayTags.Attributes_Resistance_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Resistance.Physical"),
		FString("Resistance Physical Damage Type"));

	// Map of Damage Types to Resistances
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Fire, GameplayTags.Attributes_Resistance_Fire);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Lightning, GameplayTags.Attributes_Resistance_Lightning);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Arcane, GameplayTags.Attributes_Resistance_Arcane);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Physical, GameplayTags.Attributes_Resistance_Physical);

	// Meta
	GameplayTags.Attributes_Meta_IncomingXP = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Meta.IncomingXP"),
		FString("Meta Attribute Incoming XP"));

	
	// Abilities
	GameplayTags.Ability_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.HitReact"),
		FString("Tag granted when Hit Reacting"));
	GameplayTags.Ability_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Attack"),
		FString("Tag to Attack"));
	GameplayTags.Ability_Summon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Summon"),
		FString("Tag to Summon minions"));
	GameplayTags.Ability_Fire_FireBall = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Fire.FireBall"),
		FString("Tag to fire FireBall"));

	// Cooldown
	GameplayTags.Cooldown_Fire_FireBall = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Cooldown.Fire.FireBall"),
		FString("Tag for fireball cooldown"));
	GameplayTags.Cooldown_Summon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Cooldown.Summon"),
		FString("Tag for Summon Cooldown"));
	
	// Events
	GameplayTags.Event_Weapon_Hit = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Weapon.Hit"),
		FString("If the Weapon hit an actor then trigger the event"));
	GameplayTags.Event_GetHit = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.GetHit"),
		FString("Use for choose the direction of hit reaction"));
}
