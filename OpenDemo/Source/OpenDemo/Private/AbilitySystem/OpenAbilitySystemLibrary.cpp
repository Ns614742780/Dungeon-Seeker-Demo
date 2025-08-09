#include "AbilitySystem/OpenAbilitySystemLibrary.h"

#include "AbilitySystemComponent.h"
#include "OpenAbilityTypes.h"
#include "Engine/OverlapResult.h"
#include "Game/OpenGameModeBase.h"
#include "Interfaces/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/OpenHUD.h"
#include "UI/WidgetController/OpenWidgetController.h"
#include "Player/OpenPlayerState.h"

bool UOpenAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject,
	FWidgetControllerParams& OutWCParams, AOpenHUD*& OutOpenHUD)
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		OutOpenHUD = Cast<AOpenHUD>(PlayerController->GetHUD());
		if (OutOpenHUD)
		{
			AOpenPlayerState* PlayerState = PlayerController->GetPlayerState<AOpenPlayerState>();
			UAbilitySystemComponent* AbilitySystemComponent = PlayerState->GetAbilitySystemComponent();
			UAttributeSet* AttributeSet = PlayerState->GetAttributeSet();

			OutWCParams.AbilitySystemComponent = AbilitySystemComponent;
			OutWCParams.AttributeSet = AttributeSet;
			OutWCParams.PlayerState = PlayerState;
			OutWCParams.PlayerController = PlayerController;
		
			return true;
		}
	}
	return false;
}

UOverlayWidgetController* UOpenAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetControllerParams;
	AOpenHUD* OpenHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WidgetControllerParams, OpenHUD))
	{
		return OpenHUD->GetOverlayWidgetController(WidgetControllerParams);
	}
	return nullptr;
}

UAttributeMenuWidgetController* UOpenAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetControllerParams;
	AOpenHUD* OpenHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WidgetControllerParams, OpenHUD))
	{
		return OpenHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
	}
	return nullptr;
}

void UOpenAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float CharacterLevel, UAbilitySystemComponent* AbilitySystemComponent)
{
	AActor* AvatarActor = AbilitySystemComponent->GetAvatarActor();
	
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryContextHandle = AbilitySystemComponent->MakeEffectContext();
	PrimaryContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, CharacterLevel, PrimaryContextHandle);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryContextHandle = AbilitySystemComponent->MakeEffectContext();
	SecondaryContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, CharacterLevel, SecondaryContextHandle);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle MainContextHandle = AbilitySystemComponent->MakeEffectContext();
	MainContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle MainAttributesSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(CharacterClassInfo->MainAttributes, CharacterLevel, MainContextHandle);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*MainAttributesSpecHandle.Data.Get());
}

void UOpenAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, ECharacterClass CharacterClass, UAbilitySystemComponent* ASC)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	// like hit react the common abilities needn't the level
	for (const TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
	// the attack abilities need level
	const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartupAbilities)
	{
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

UCharacterClassInfo* UOpenAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const AOpenGameModeBase* OpenGameMode = Cast<AOpenGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (OpenGameMode == nullptr) { return nullptr; }
	return OpenGameMode->CharacterClassInfo;
}

bool UOpenAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FOpenGameplayEffectContext* OpenGameplayEffectContext = static_cast<const FOpenGameplayEffectContext*>(ContextHandle.Get()))
	{
		return OpenGameplayEffectContext->IsBlockedHit();
	}
	return false;
}

bool UOpenAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FOpenGameplayEffectContext* OpenGameplayEffectContext = static_cast<const FOpenGameplayEffectContext*>(ContextHandle.Get()))
	{
		return OpenGameplayEffectContext->IsCriticalHit();
	}
	return false;
}

void UOpenAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& ContextHandle, bool bInIsBlockedHit)
{
	if (FOpenGameplayEffectContext* OpenGameplayEffectContext = static_cast<FOpenGameplayEffectContext*>(ContextHandle.Get()))
	{
		OpenGameplayEffectContext->SetBlockedHit(bInIsBlockedHit);
	}
}

void UOpenAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& ContextHandle, bool bInIsCriticalHit)
{
	if (FOpenGameplayEffectContext* OpenGameplayEffectContext = static_cast<FOpenGameplayEffectContext*>(ContextHandle.Get()))
	{
		OpenGameplayEffectContext->SetCriticalHit(bInIsCriticalHit);
	}
}

void UOpenAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject,
	TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore,
	float Radius, const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);
	TArray<FOverlapResult> Overlaps;
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity,
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
			FCollisionShape::MakeSphere(Radius), SphereParams);
		
		for (FOverlapResult& Overlap : Overlaps)
		{
			// check if the actor has combat interface and is alive 
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}
		}
	}
}

int32 UOpenAbilitySystemLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) { return 0; }

	const FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	const float XPReward = ClassDefaultInfo.XPReward.GetValueAtLevel(CharacterLevel);

	return static_cast<int32>(XPReward);
}

bool UOpenAbilitySystemLibrary::IsFriend(const AActor* SourceActor, const AActor* TargetActor)
{
	return (SourceActor->ActorHasTag(FName("Player")) && TargetActor->ActorHasTag(FName("Player"))) ||
		SourceActor->ActorHasTag(FName("Enemy")) && TargetActor->ActorHasTag(FName("Enemy"));
}
