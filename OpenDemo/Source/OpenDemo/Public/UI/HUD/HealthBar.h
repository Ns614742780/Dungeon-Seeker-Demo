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
	UPROPERTY(meta = (BindWidget))	//����ͼ�е�Widget�󶨣�ע��Ҫ����ͼ������Widget������ΪHealthBar�����Ʊ���һ��
	class UProgressBar* HealthBar;
};
