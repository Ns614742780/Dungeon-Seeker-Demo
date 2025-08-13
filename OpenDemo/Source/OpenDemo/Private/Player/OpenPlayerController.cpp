#include "Player/OpenPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "MovieSceneTracksComponentTypes.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "AbilitySystem/OpenAbilitySystemComponent.h"
#include "Characters/OpenCharacter.h"
#include "Input/OpenInputComponent.h"
#include "OpenDemo/OpenLogChannels.h"
#include "UI/Widget/DamageTextComponent.h"


AOpenPlayerController::AOpenPlayerController()
{
	bReplicates = true;
}

void AOpenPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(OpenInputMappingContext);
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(OpenInputMappingContext, 0);
	}

	bShowMouseCursor = false;
}

void AOpenPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UOpenInputComponent * OpenInputComponent = CastChecked<UOpenInputComponent>(InputComponent);

	OpenInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOpenPlayerController::Move);
	OpenInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &AOpenPlayerController::Turn);
	OpenInputComponent->BindAction(LookUpAction, ETriggerEvent::Triggered, this, &AOpenPlayerController::LookUp);

	OpenInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AOpenPlayerController::ShiftPressed);
	OpenInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AOpenPlayerController::ShiftReleased);
	OpenInputComponent->BindAction(AltAction, ETriggerEvent::Started, this, &AOpenPlayerController::AltPressed);
	OpenInputComponent->BindAction(AltAction, ETriggerEvent::Completed, this, &AOpenPlayerController::AltReleased);
	OpenInputComponent->BindAction(InteractionAction, ETriggerEvent::Started, this, &AOpenPlayerController::EKeyPressed);
	OpenInputComponent->BindAction(WeaponChange, ETriggerEvent::Started, this, &AOpenPlayerController::QKeyPressed);
	
	OpenInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AOpenPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, const bool bBlockedHit, const bool bCriticalHit)
{
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
	}
}

UOpenAbilitySystemComponent* AOpenPlayerController::GetASC()
{
	if (OpenAbilitySystemComponent == nullptr)
	{
		OpenAbilitySystemComponent = Cast<UOpenAbilitySystemComponent>(
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return OpenAbilitySystemComponent;
}

void AOpenPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetASC() == nullptr) { return; }
	GetASC()->AbilityInputTagHeld(InputTag);
}

void AOpenPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC() == nullptr) { return; }
	GetASC()->AbilityInputTagReleased(InputTag);
}

void AOpenPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetASC() == nullptr) { return; }
	GetASC()->AbilityInputTagHeld(InputTag);
}

void AOpenPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	
	const FRotator ControllerRotation = GetControlRotation();
	const FRotator YawRotation(0.f, ControllerRotation.Yaw, 0.f);
	
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AOpenPlayerController::Turn(const FInputActionValue& InputActionValue)
{
	const float InputValue = InputActionValue.Get<float>();
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddControllerYawInput(InputValue);
	}
}

void AOpenPlayerController::LookUp(const FInputActionValue& InputActionValue)
{
	const float InputValue = InputActionValue.Get<float>();
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddControllerPitchInput(InputValue);
	}
}

void AOpenPlayerController::ShiftPressed()
{
	GetCharacter()->GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AOpenPlayerController::ShiftReleased()
{
	GetCharacter()->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AOpenPlayerController::AltPressed()
{
	bShowMouseCursor = true;
	IgnoreLookInput = true;
}

void AOpenPlayerController::AltReleased()
{
	bShowMouseCursor = false;
	IgnoreLookInput = false;
}

void AOpenPlayerController::EKeyPressed()
{
	UE_LOG(LogOpen, Display, TEXT("Press e"));
	if (AOpenCharacter* OpenCharacter = Cast<AOpenCharacter>(GetCharacter()))
	{
		OpenCharacter->HandleEKey();
	}
}

void AOpenPlayerController::QKeyPressed()
{
	if (AOpenCharacter* OpenCharacter = Cast<AOpenCharacter>(GetCharacter()))
	{
		OpenCharacter->HandleQKey();
	}
}
