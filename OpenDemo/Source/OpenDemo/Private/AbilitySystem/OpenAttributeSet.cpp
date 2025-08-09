#include "AbilitySystem/OpenAttributeSet.h"

#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"
#include "OpenGameplayTags.h"
#include "AbilitySystem/OpenAbilitySystemComponent.h"
#include "AbilitySystem/OpenAbilitySystemLibrary.h"
#include "Interfaces/CombatInterface.h"
#include "Interfaces/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "OpenDemo/OpenLogChannels.h"
#include "Player/OpenPlayerController.h"

UOpenAttributeSet::UOpenAttributeSet()
{
	const FOpenGameplayTags& GameplayTags = FOpenGameplayTags::Get();

	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);

	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Armor, GetArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ArmorBlockChance, GetBlockChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_PowerRegeneration, GetPowerRegenerationAttribute);
	
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxPower, GetMaxPowerAttribute);

	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Arcane, GetArcaneResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Fire, GetFireResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Lightning, GetLightningResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Physical, GetPhysicalResistanceAttribute);

}

void UOpenAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// 设置回调函数用于在值改变的时候被触发
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, Power, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, MaxPower, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);	
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, PowerRegeneration, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
}

void UOpenAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

}

void UOpenAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// Data中有非常多可用的数据,包括 source 和 target
	FEffectProperties Props;
	SetEffectProperties(Data, Props);
	
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// 设置health的数值使其不超过最大值
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetPowerAttribute())
	{
		SetPower(FMath::Clamp(GetPower(), 0.0f, GetMaxPower()));
	}

	if (Data.EvaluatedData.Attribute == GetStrengthAttribute())
	{
		UE_LOG(LogOpen, Warning, TEXT("[ AttributeSet test ] apply to %s Strength modified to: %f"), *GetOwningActor()->GetName(), GetStrength());
		UE_LOG(LogTemp, Warning, TEXT("Strength PostGEExecute: Base=%f, Current=%f"),
			Strength.GetBaseValue(), Strength.GetCurrentValue());
	}
	
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		if (UOpenAbilitySystemLibrary::IsFriend(Props.SourceAvatarActor, Props.TargetAvatarActor)) { return; }
		const float LocalIncomingDamage = GetIncomingDamage();
		SetIncomingDamage(0.f);

		if (LocalIncomingDamage > 0)
		{
			const float NewHealth = GetHealth() - LocalIncomingDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));
			
			if (const bool bFatal = NewHealth <= 0.f)
			{
				if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor))
				{
					CombatInterface->Die();
				}
				SendXPEvent(Props);
			}
			else
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FOpenGameplayTags::Get().Ability_HitReact);
				// activate hit react ability
				
				bool success = Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);

				if (!success)
				{
					UE_LOG(LogTemp, Log, TEXT("ACTIVATE ABILITY NOT SUCCESS"))
				}
				// send the location from the attack, send the data by event
				FGameplayEventData EventGetHitData;
				EventGetHitData.EventTag = FOpenGameplayTags::Get().Event_GetHit;
				EventGetHitData.Instigator = Props.SourceAvatarActor;
				GetOwningAbilitySystemComponent()->HandleGameplayEvent(EventGetHitData.EventTag, &EventGetHitData);
				
			}

			const bool bBlock = UOpenAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
			const bool bCriticalHit = UOpenAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
			ShowFloatingText(Props, LocalIncomingDamage, bBlock, bCriticalHit);
		}
	}

	if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		const float LocalIncomingXP = GetIncomingXP();
		SetIncomingXP(0.f);

		// source character of the incoming XP is the player itself
		if (Props.SourceCharacter->Implements<UPlayerInterface>() && Props.SourceCharacter->Implements<UCombatInterface>())
		{
			const int32 CurrentLevel = ICombatInterface::Execute_GetPlayerLevel(Props.SourceCharacter);
			const int32 CurrentXP = IPlayerInterface::Execute_GetXP(Props.SourceCharacter);

			const int32 NewLevel = IPlayerInterface::Execute_FindLevelForXP(Props.SourceCharacter, CurrentXP + LocalIncomingXP);
			const int32 LevelUpNum = NewLevel - CurrentLevel;

			if (LevelUpNum > 0)
			{
				int32 AttributePointsReward = IPlayerInterface::Execute_GetAttributePointReward(Props.SourceCharacter, CurrentLevel);
				int32 SpellPointsReward = IPlayerInterface::Execute_GetSpellPointReward(Props.SourceCharacter, CurrentLevel);

				IPlayerInterface::Execute_AddToPlayerLevel(Props.SourceCharacter, LevelUpNum);
				IPlayerInterface::Execute_AddToAttributePoints(Props.SourceCharacter, AttributePointsReward);
				IPlayerInterface::Execute_AddToSpellPoints(Props.SourceCharacter, SpellPointsReward);

				bLevelUpHealthChange = true;
				bLevelUpPowerChange = true;
				
				IPlayerInterface::Execute_LevelUp(Props.SourceCharacter);
			}
			
			IPlayerInterface::Execute_AddToXP(Props.SourceCharacter, LocalIncomingXP);
		}
	}
	
}

void UOpenAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute() && bLevelUpHealthChange)
	{
		SetHealth(GetMaxHealth());
		bLevelUpHealthChange = false;
	}
	if (Attribute == GetMaxPowerAttribute() && bLevelUpPowerChange)
	{
		SetPower(GetMaxPower());
		bLevelUpPowerChange = false;
	}
}

void UOpenAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOpenAttributeSet, Health, OldHealth);
}

void UOpenAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOpenAttributeSet, MaxHealth, OldMaxHealth);
}

void UOpenAttributeSet::OnRep_Power(const FGameplayAttributeData& OldPower) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOpenAttributeSet, Power, OldPower);
}

void UOpenAttributeSet::OnRep_MaxPower(const FGameplayAttributeData& OldMaxPower) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOpenAttributeSet, MaxPower, OldMaxPower);
}

void UOpenAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOpenAttributeSet, Strength, OldStrength);
}

void UOpenAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOpenAttributeSet, Intelligence, OldIntelligence);
}

void UOpenAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOpenAttributeSet, Resilience, OldResilience);
}

void UOpenAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOpenAttributeSet, Vigor, OldVigor);
}

void UOpenAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOpenAttributeSet, Armor, OldArmor);
}
void UOpenAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOpenAttributeSet, ArmorPenetration, OldArmorPenetration);
}
void UOpenAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOpenAttributeSet, BlockChance, OldBlockChance);
}
void UOpenAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOpenAttributeSet, CriticalHitChance, OldCriticalHitChance);
}
void UOpenAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOpenAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}
void UOpenAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOpenAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}
void UOpenAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOpenAttributeSet, HealthRegeneration, OldHealthRegeneration);
}
void UOpenAttributeSet::OnRep_PowerRegeneration(const FGameplayAttributeData& OldPowerRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOpenAttributeSet, PowerRegeneration, OldPowerRegeneration);
}

void UOpenAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOpenAttributeSet, FireResistance, OldFireResistance);
}

void UOpenAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOpenAttributeSet, LightningResistance, OldLightningResistance);
}

void UOpenAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOpenAttributeSet, ArcaneResistance, OldArcaneResistance);
}

void UOpenAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOpenAttributeSet, PhysicalResistance, OldPhysicalResistance);
}


void UOpenAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() &&
		Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();

		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

void UOpenAttributeSet::ShowFloatingText(const FEffectProperties& Props, const float Damage, const bool bBlockedHit, const bool bCriticalHit) const
{
	if (Props.SourceCharacter != Props.TargetCharacter)
	{
		if (AOpenPlayerController* PC = Cast<AOpenPlayerController>(Props.SourceCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
			return;
		}
		if (AOpenPlayerController* PC = Cast<AOpenPlayerController>(Props.TargetCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
		}
	}
}

void UOpenAttributeSet::SendXPEvent(const FEffectProperties& Props)
{
	if (Props.TargetCharacter->Implements<UCombatInterface>())
	{
		const int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(Props.TargetCharacter);
		const ECharacterClass TargetCharacterClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetCharacter);
		const int32 XPReward = UOpenAbilitySystemLibrary::GetXPRewardForClassAndLevel(Props.TargetCharacter, TargetCharacterClass, TargetLevel);

		const FOpenGameplayTags& GameplayTags = FOpenGameplayTags::Get();
		FGameplayEventData Payload;
		Payload.EventTag = GameplayTags.Attributes_Meta_IncomingXP;
		Payload.EventMagnitude = XPReward;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, GameplayTags.Attributes_Meta_IncomingXP, Payload);
	}
}
