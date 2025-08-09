// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OpenOverlay.generated.h"

/**
 * 
 */
UCLASS()
class OPENDEMO_API UOpenOverlay : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetHealthBarPercent(float Percent);
	void SetPowerBarPercent(float Percent);
	void SetGold(int32 Gold);

private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* PowerBar;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PowerText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GoldText;
};
