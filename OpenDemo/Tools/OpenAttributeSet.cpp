#include "AbilitySystem/OpenAttributeSet.h"

#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"

UOpenAttributeSet::UOpenAttributeSet()
{
	InitHealth(50.0f);
	InitMaxHealth(100.0f);
}

void UOpenAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// ע��Health���Ե�Replicated������ע����ܱ�Replicated
	// COND_None��ʾ���������ƣ�REPNOTIFY_Always��ʾֻҪ���������Է����仯���ͻ�֪ͨ�ͻ���
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, Power, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, MaxPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOpenAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

}

void UOpenAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

}

void UOpenAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// Sourse��Ч���ķ����ߣ�Target��Ч���ĳ�����
	// Data���зǳ��ḻ����Ϣ������EffectSpec��Target��Source��
	// ���ҿ���ͨ��Data.EvaluatedData����ȡ���޸ĵ�����ֵ
	FEffectProperties Props;
	SetEffectProperties(Data, Props);


	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// ���Health���Է����仯����������ֵ��0��MaxHealth֮��
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
}
void UOpenAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	// �����Լ���Ϊ���Ƶ���������RepNotifyʱ������֪ͨAbilitySystem�ñ仯�����ʹ������ĺ�
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

///HERE GENERATE FUNCTION IMPLEMENTATION
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