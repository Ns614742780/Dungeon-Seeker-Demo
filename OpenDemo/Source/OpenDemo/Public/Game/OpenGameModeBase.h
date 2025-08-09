#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OpenGameModeBase.generated.h"


class UWidgetComponent;
class AOpenCharacter;
class AOpenPlayerController;
class AGeneratedMap;
class UCharacterClassInfo;

UCLASS()
class OPENDEMO_API AOpenGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AOpenGameModeBase();
	
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void SpawnPlayerAfterMapGenerated(const FVector& SpawnLocation);

	UFUNCTION(BlueprintCallable, Category = "Victory")
	void WinGame();

	UFUNCTION(BlueprintCallable, Category = "Victory")
	void LoseGame();

	UFUNCTION(BlueprintCallable, Category = "Victory")
	void HandleLoseGame();

	FTimerHandle LoseTimer;
	
	UPROPERTY(EditDefaultsOnly, Category = "Victory")
	TSubclassOf<UUserWidget> WinWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Victory")
	TSubclassOf<UUserWidget> LoseWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Victory")
	TObjectPtr<USoundBase> WinSound;

	UPROPERTY(EditDefaultsOnly, Category = "Victory")
	TObjectPtr<USoundBase> LoseSound;
	
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSubclassOf<AGeneratedMap> GeneratedMapClass;
	
	UPROPERTY(VisibleAnywhere, Category = "Map")
	TObjectPtr<AGeneratedMap> GeneratedMap;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	bool bDebugMode = false;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	bool bNeedPlayerSpawn = false;
	
};
