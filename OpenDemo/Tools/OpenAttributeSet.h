#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"

#include "OpenAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()
	
	FEffectProperties() {};


	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;
	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;
	UPROPERTY()
	AController* SourceController = nullptr;
	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;
	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;
	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;
	UPROPERTY()
	AController* TargetController = nullptr;
	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};

UCLASS()
class OPENDEMO_API UOpenAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UOpenAttributeSet();

	// ���һ������Ϊreplication����Ҫ�ú������������Ϊreplicationע�����
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// �ú����������Է����仯��Ч֮ǰ�����ã���������������Խ���Ԥ������������
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	// �ú����������Է����仯�󱻵��ã���������������Խ��к���
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;


	// Primary attributes
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "Primary Attributes")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UOpenAttributeSet, Strength)
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category = "Primary Attributes")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UOpenAttributeSet, Intelligence)
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Resilience, Category = "Primary Attributes")
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(UOpenAttributeSet, Resilience)
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Vigor, Category = "Primary Attributes")
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(UOpenAttributeSet, Vigor)


	// Main attributes
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Main Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UOpenAttributeSet, Health)
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Main Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UOpenAttributeSet, MaxHealth)
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Power, Category = "Main Attributes")
	FGameplayAttributeData Power;
	ATTRIBUTE_ACCESSORS(UOpenAttributeSet, Power)
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxPower, Category = "Main Attributes")
	FGameplayAttributeData MaxPower;
	ATTRIBUTE_ACCESSORS(UOpenAttributeSet, MaxPower)

///HERE GENERATE PROPERTY
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Secondary Attributes")
FGameplayAttributeData Armor;
ATTRIBUTE_ACCESSORS(UOpenAttributeSet, Armor)
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetration, Category = "Secondary Attributes")
FGameplayAttributeData ArmorPenetration;
ATTRIBUTE_ACCESSORS(UOpenAttributeSet, ArmorPenetration)
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BlockChance, Category = "Secondary Attributes")
FGameplayAttributeData BlockChance;
ATTRIBUTE_ACCESSORS(UOpenAttributeSet, BlockChance)
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitChance, Category = "Secondary Attributes")
FGameplayAttributeData CriticalHitChance;
ATTRIBUTE_ACCESSORS(UOpenAttributeSet, CriticalHitChance)
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitDamage, Category = "Secondary Attributes")
FGameplayAttributeData CriticalHitDamage;
ATTRIBUTE_ACCESSORS(UOpenAttributeSet, CriticalHitDamage)
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitResistance, Category = "Secondary Attributes")
FGameplayAttributeData CriticalHitResistance;
ATTRIBUTE_ACCESSORS(UOpenAttributeSet, CriticalHitResistance)
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegeneration, Category = "Secondary Attributes")
FGameplayAttributeData HealthRegeneration;
ATTRIBUTE_ACCESSORS(UOpenAttributeSet, HealthRegeneration)
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PowerRegeneration, Category = "Secondary Attributes")
FGameplayAttributeData PowerRegeneration;
ATTRIBUTE_ACCESSORS(UOpenAttributeSet, PowerRegeneration)





	// �����RepNotify��ʹ�������ԣ���ô�ͻ������Եľ�ֵ
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	UFUNCTION()
	void OnRep_Power(const FGameplayAttributeData& OldPower) const;
	UFUNCTION()
	void OnRep_MaxPower(const FGameplayAttributeData& OldMaxPower) const;
	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldStrength) const;
	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const;
	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const;
	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldVigor) const;

UFUNCTION()
void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;
UFUNCTION()
void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const;
UFUNCTION()
void OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const;
UFUNCTION()
void OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const;
UFUNCTION()
void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const;
UFUNCTION()
void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const;
UFUNCTION()
void OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const;
UFUNCTION()
void OnRep_PowerRegeneration(const FGameplayAttributeData& OldPowerRegeneration) const;

///HERE GENERATE FUNCTION

private:

	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
};