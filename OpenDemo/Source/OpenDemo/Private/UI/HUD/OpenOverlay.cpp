// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/OpenOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UOpenOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(Percent);
	}
}

void UOpenOverlay::SetPowerBarPercent(float Percent)
{
	if (PowerBar)
	{
		PowerBar->SetPercent(Percent);
	}
}

void UOpenOverlay::SetGold(int32 Gold)
{
	if (GoldText)
	{
		GoldText->SetText(FText::AsNumber(Gold));
	}
}
