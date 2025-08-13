// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

/**
 * 
 */
UCLASS()
class OPENDEMO_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))	//与蓝图中的Widget绑定，注意要在蓝图中设置Widget的名称为HealthBar，名称必须一致
	class UProgressBar* HealthBar;
};
