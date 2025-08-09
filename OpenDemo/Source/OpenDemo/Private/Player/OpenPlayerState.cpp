#include "Player/OpenPlayerState.h"

#include "AbilitySystem/OpenAbilitySystemComponent.h"
#include "AbilitySystem/OpenAttributeSet.h"
#include "Net/UnrealNetwork.h"

AOpenPlayerState::AOpenPlayerState()
{
	NetUpdateFrequency = 100.0f; 
	
	AbilitySystemComponent = CreateDefaultSubobject<UOpenAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UOpenAttributeSet>(TEXT("AttributeSet"));

}

void AOpenPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AOpenPlayerState, PlayerLevel);
	DOREPLIFETIME(AOpenPlayerState, PlayerXP);
	DOREPLIFETIME(AOpenPlayerState, AttributePoint);
	DOREPLIFETIME(AOpenPlayerState, SpellPoint);
}

UAbilitySystemComponent* AOpenPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* AOpenPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

void AOpenPlayerState::AddToXP(int32 InXP)
{
	PlayerXP += InXP;
	OnPlayerXPChangedDelegate.Broadcast(PlayerXP);
}

void AOpenPlayerState::AddToLevel(int32 InLevel)
{
	PlayerLevel += InLevel;
	OnPlayerLevelChangedDelegate.Broadcast(PlayerLevel);
}

void AOpenPlayerState::AddToAttributePoints(int32 InPoints)
{
	AttributePoint += InPoints;
	OnAttributePointChangedDelegate.Broadcast(AttributePoint);
}

void AOpenPlayerState::AddToSpellPoints(int32 InPoints)
{
	SpellPoint += InPoints;
	OnSpellPointChangedDelegate.Broadcast(SpellPoint);
}

void AOpenPlayerState::SetXP(int32 InXP)
{
	PlayerXP = InXP;
	OnPlayerXPChangedDelegate.Broadcast(PlayerXP);
}

void AOpenPlayerState::SetLevel(int32 InLevel)
{
	PlayerLevel = InLevel;
	OnPlayerLevelChangedDelegate.Broadcast(PlayerLevel);
}

void AOpenPlayerState::OnRep_Level(int32 OldLevel)
{
	OnPlayerLevelChangedDelegate.Broadcast(PlayerLevel);
}

void AOpenPlayerState::OnRep_XP(int32 OldXP)
{
	OnPlayerXPChangedDelegate.Broadcast(PlayerXP);
}
void AOpenPlayerState::OnRep_AttributePoint(int32 OldAttributePoint)
{
	OnAttributePointChangedDelegate.Broadcast(AttributePoint);
}

void AOpenPlayerState::OnRep_SpellPoint(int32 OldSpellPoint)
{
	OnSpellPointChangedDelegate.Broadcast(SpellPoint);
}