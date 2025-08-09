#include "Game/OpenGameModeBase.h"

#include "Components/WidgetComponent.h"
#include "Engine/RendererSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Map/GeneratedMap.h"
#include "OpenDemo/OpenLogChannels.h"

AOpenGameModeBase::AOpenGameModeBase()
{
	bStartPlayersAsSpectators = true;
}

void AOpenGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (GeneratedMapClass && !bDebugMode)
	{
		GeneratedMap = GetWorld()->SpawnActor<AGeneratedMap>(
			GeneratedMapClass, FVector(0,0,200.f), FRotator::ZeroRotator);
	}

	if (GeneratedMap && !bDebugMode)
	{
		UE_LOG(LogOpen, Display, TEXT("[ GameMode ] Binding event to GeneratedMap"));
		GeneratedMap->OnMapGeneratedFinish.AddDynamic(this, &AOpenGameModeBase::SpawnPlayerAfterMapGenerated);
	}

	if (bNeedPlayerSpawn)
	{
		SpawnPlayerAfterMapGenerated(FVector(0,0,0));
	}
}

void AOpenGameModeBase::SpawnPlayerAfterMapGenerated(const FVector& SpawnLocation)
{
	if (bDebugMode && !bNeedPlayerSpawn) { return; }
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SpawnLocation);
		SpawnTransform.SetRotation(FQuat::Identity);
		
		if (DefaultPawnClass)
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = PC;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			
			APawn* Character = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, SpawnTransform, SpawnParameters);
            UE_LOG(LogOpen, Warning, TEXT("Create pawn %s"), *Character->GetName());
            if (Character)
            {
            	PC->Possess(Character);
            }
		}
	}
}

void AOpenGameModeBase::WinGame()
{
	if (WinSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), WinSound);
	}
	if (WinWidgetClass)
	{
		if (UUserWidget* WinWidget = CreateWidget<UUserWidget>(GetWorld(), WinWidgetClass))
		{
			WinWidget->AddToViewport();
		}
	}

	UGameplayStatics::SetGamePaused(GetWorld(), true);
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PlayerController->SetInputMode(InputMode);

		PlayerController->bShowMouseCursor = true;
	}
}

void AOpenGameModeBase::LoseGame()
{
	GetWorld()->GetTimerManager().SetTimer(
		LoseTimer,
		this,
		&AOpenGameModeBase::HandleLoseGame,
		1.0f,
		false);
}

void AOpenGameModeBase::HandleLoseGame()
{
	if (LoseSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), LoseSound);
	}
	if (LoseWidgetClass)
	{
		if (UUserWidget* LoseWidget = CreateWidget<UUserWidget>(GetWorld(), LoseWidgetClass))
		{
			LoseWidget->AddToViewport();
		}
	}

	UGameplayStatics::SetGamePaused(GetWorld(), true);
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PlayerController->SetInputMode(InputMode);

		PlayerController->bShowMouseCursor = true;
	}
}
