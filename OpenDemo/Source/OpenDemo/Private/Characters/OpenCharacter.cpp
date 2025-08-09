#include "Characters/OpenCharacter.h"

#include "NiagaraComponent.h"
#include "OpenGameplayTags.h"
#include "Player/OpenPlayerState.h"
#include "Player/OpenPlayerController.h"
#include "AbilitySystem/OpenAbilitySystemComponent.h"
#include "AbilitySystem/OpenAttributeSet.h"
#include "AbilitySystem/Data/LevelUpInfo.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AttributeComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Items/ItemTypes.h"
#include "Animation/AnimMontage.h"
#include "Components/AudioComponent.h"
#include "Game/OpenGameModeBase.h"
#include "Items/EndDoor.h"
#include "Items/GameKey.h"
#include "UI/HUD/OpenHUD.h"
#include "UI/Widget/OpenUserWidget.h"

AOpenCharacter::AOpenCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	MeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	MeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	MeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);

	//AutoPossessPlayer = EAutoReceiveInput::Player0;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->TargetArmLength = 200.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	LevelUpCascadeComponent = CreateDefaultSubobject<UParticleSystemComponent>("LevelUpCascadeComponent");
	LevelUpCascadeComponent->SetupAttachment(RootComponent);
	LevelUpCascadeComponent->bAutoActivate = false;

	BGMComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("BGMComponent"));
	BGMComponent->SetupAttachment(RootComponent);
	BGMComponent->bAutoActivate = false;
	
	CharacterClass = ECharacterClass::Player;
}

void AOpenCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (ViewCamera)
	{
		FPostProcessSettings PostProcessSettings = ViewCamera->PostProcessSettings;
		PostProcessSettings.bOverride_AutoExposureMinBrightness = true;
		PostProcessSettings.AutoExposureMinBrightness = 0.05f;
		PostProcessSettings.bOverride_AutoExposureMaxBrightness = true;
		PostProcessSettings.AutoExposureMaxBrightness = 0.3f;
		PostProcessSettings.AutoExposureMethod = AEM_Manual;
		ViewCamera->PostProcessSettings = PostProcessSettings;
	}
}

void AOpenCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Tags.Add(FName("Player"));
	
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	if (BGMComponent && BGMSound)
	{
		BGMComponent->SetSound(BGMSound);
		if (!BGMComponent->IsPlaying())
		{
			BGMComponent->Play();
			BGMComponent->FadeIn(2.0f, 1);
		}
	}

}

void AOpenCharacter::HandleEKey()
{
	AGameKey* OverlappingKey = Cast<AGameKey>(OverlappingItem);
	if (OverlappingKey)
	{
		bHasKey = true;
		OverlappingKey->Destroy();
		return;
	}
	AEndDoor* OverlappingDoor = Cast<AEndDoor>(OverlappingItem);
	if (OverlappingDoor)
	{
		if (bHasKey)
		{
			if (AOpenGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AOpenGameModeBase>())
			{
				GameMode->WinGame();
			}
		}
		return;
	}
	
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem); // Check if the overlapping item is a weapon
	if (OverlappingWeapon)
	{
		Weapons.AddUnique(OverlappingWeapon);
		ActiveWeaponIndex = Weapons.Num() - 1; // Set the active weapon index to the last added weapon

		if (CharacterState == ECharacterState::ECS_Unequipped)
		{
			CharacterState = ECharacterState::ECS_HasWeaponUnequipped;
			OverlappingWeapon->EquipWeapon(GetMesh(), FName("BackWeapon"), this, this);
			DisArm();
		}
		else if (CharacterState == ECharacterState::ECS_HasWeaponUnequipped)
		{
			OverlappingWeapon->EquipWeapon(GetMesh(), FName("BackWeapon"), this, this);
			DisArm();
		}
		else
		{
			OverlappingWeapon->EquipWeapon(GetMesh(), OverlappingWeapon->GetWeaponSocketName(), this, this);
			Equip();
		}
		
	}
	else if(CharacterState != ECharacterState::ECS_Unequipped)
	{
		PlayEquipMontage();
	}
	
}

void AOpenCharacter::HandleQKey()
{
	if (Weapons.Num() > 1)
	{
		ActiveWeaponIndex = (ActiveWeaponIndex + 1) % Weapons.Num();
		for (AWeapon* Weapon : Weapons)
		{
			Weapon->SetVisibility(Weapon == Weapons[ActiveWeaponIndex]);
		}
		SetCharacterStateByActiveWeapon();
	}
}

void AOpenCharacter::Attack()
{
	Super::Attack(); // Call the base class Attack function
	if (CanAttack())
	{
		ActionState = EActionState::EAS_Attacking; // Set action state to attacking
		PlayAttackMontage();
	}
}

bool AOpenCharacter::CanAttack() const
{
	return (ActionState == EActionState::EAS_Unoccupied) &&
		(CharacterState == ECharacterState::ECS_EquippedRightHand);
}

void AOpenCharacter::PlayEquipMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		EWeaponType WeaponType = Weapons[ActiveWeaponIndex]->GetWeaponType();
		ActionState = EActionState::EAS_Equipping; // Set action state to interacting
		if (CharacterState == ECharacterState::ECS_HasWeaponUnequipped) {
			AnimInstance->Montage_Play(EquipMontage, 1.0f);
			switch (WeaponType)
			{
			case EWeaponType::EWT_LeftHanded:
				break;
			case EWeaponType::EWT_RightHanded:
				AnimInstance->Montage_JumpToSection(FName("EquipRight"), EquipMontage); // Jump to the unequip section
				CharacterState = ECharacterState::ECS_EquippedRightHand;
				break;
			case EWeaponType::EWT_TwoHanded:
				AnimInstance->Montage_JumpToSection(FName("EquipBothHanded"), EquipMontage); // Jump to the unequip section
				CharacterState = ECharacterState::ECS_EquippedBothHands; // Update character state to Equipped both hands
				break;
			case EWeaponType::EWT_Body:
				AnimInstance->Montage_JumpToSection(FName("EquipBody"), EquipMontage); // Jump to the unequip section
				CharacterState = ECharacterState::ECS_EquippedLeftHand; // Update character state to Equipped left hand
				break;
			case EWeaponType::EWT_NULL:
				break;
			default:
				break;
			}
		}
		else if (CharacterState == ECharacterState::ECS_EquippedRightHand) {
			AnimInstance->Montage_Play(EquipMontage, 1.0f);
			AnimInstance->Montage_JumpToSection(FName("UnEquipRight"), EquipMontage); // Jump to the equip section
			CharacterState = ECharacterState::ECS_HasWeaponUnequipped; // Update character state to unequipped
		}
		else if (CharacterState == ECharacterState::ECS_EquippedBothHands) {
			AnimInstance->Montage_Play(EquipMontage, 1.0f);
			AnimInstance->Montage_JumpToSection(FName("UnEquipBothHanded"), EquipMontage); // Jump to the equip section
			CharacterState = ECharacterState::ECS_HasWeaponUnequipped; // Update character state to unequipped
		}
		else {
			return; // If character is not in a valid state, do nothing
		}
	}
}

void AOpenCharacter::Die()
{
	Super::Die();
	if (AOpenGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AOpenGameModeBase>())
	{
		GameMode->LoseGame();
	}
}

void AOpenCharacter::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

void AOpenCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied; // Reset action state to unoccupied
}

void AOpenCharacter::EquipEnd()
{
	ActionState = EActionState::EAS_Unoccupied; // Reset action state to unoccupied
}

void AOpenCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied; // Reset action state to unoccupied
}

void AOpenCharacter::DisArm()
{
	for (AWeapon* Weapon : Weapons)
	{
		Weapon->AttachMeshToSocket(GetMesh(), FName("BackWeapon")); // Disable the sphere collision of the equipped weapon
		Weapon->SetVisibility(Weapon == Weapons[ActiveWeaponIndex]);
	}
	CharacterState = ECharacterState::ECS_HasWeaponUnequipped;
}

void AOpenCharacter::Equip()
{
	for (AWeapon* Weapon : Weapons)
	{
		Weapon->AttachMeshToSocket(GetMesh(), Weapon->GetWeaponSocketName()); // Disable the sphere collision of the equipped weapon
		Weapon->SetVisibility(Weapon == Weapons[ActiveWeaponIndex]);
	}
	SetCharacterStateByActiveWeapon();
}

void AOpenCharacter::SetCharacterStateByActiveWeapon()
{
	if (Weapons.Num() <= 0) { return; }
	if (CharacterState == ECharacterState::ECS_Unequipped || CharacterState == ECharacterState::ECS_HasWeaponUnequipped) { return; }
	if (Weapons[ActiveWeaponIndex]->GetWeaponType() == EWeaponType::EWT_RightHanded)
	{
		CharacterState = ECharacterState::ECS_EquippedRightHand;
	}
	else if (Weapons[ActiveWeaponIndex]->GetWeaponType() == EWeaponType::EWT_TwoHanded)
	{
		CharacterState = ECharacterState::ECS_EquippedBothHands;
	}
}

void AOpenCharacter::MulticastLevelUpNiagara_Implementation() const
{
	if (IsValid(LevelUpCascadeComponent))
	{
		LevelUpCascadeComponent->Activate(true);
	}
}

void AOpenCharacter::InitAbilityActorInfo()
{
	AOpenPlayerState* OpenPlayerState = GetPlayerState<AOpenPlayerState>();
	check(OpenPlayerState);
	OpenPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(OpenPlayerState, this);
	
	Cast<UOpenAbilitySystemComponent>(OpenPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	
	AbilitySystemComponent = OpenPlayerState->GetAbilitySystemComponent();
	AttributeSet = OpenPlayerState->GetAttributeSet();
	
	if (AOpenPlayerController* OpenPlayerController = Cast<AOpenPlayerController>(GetController()))
	{
		OpenPlayerController->SetWalkSpeed(WalkSpeed);
		OpenPlayerController->SetRunSpeed(RunSpeed);

		if (AOpenHUD* OpenHUD = Cast<AOpenHUD>(OpenPlayerController->GetHUD()))
		{
			OpenHUD->InitOverlay(OpenPlayerController, OpenPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
}

int32 AOpenCharacter::GetPlayerLevel_Implementation()
{
	const AOpenPlayerState* OpenPlayerState = GetPlayerState<AOpenPlayerState>();
	check(OpenPlayerState);
	return OpenPlayerState->GetPlayerLevel();
}

float AOpenCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount); // Call the base class HandleDamage function
	return DamageAmount;
}

void AOpenCharacter::Jump()
{
	if (ActionState == EActionState::EAS_Unoccupied)
	{
		Super::Jump(); // Call the base class Jump function
	}
}


// Called to bind functionality to input
void AOpenCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AOpenCharacter::Jump);
}

void AOpenCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController); // Call the base class PossessedBy function
	
	// Init ability actor info for the Server
	
	InitAbilityActorInfo();
	InitializeDefaultAttributes();
	AddCharacterAbilities();
}

void AOpenCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	// Init ability actor info for the Client
	InitAbilityActorInfo();

}

void AOpenCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item; // Set the overlapping item
}

bool AOpenCharacter::HasKey()
{
	return bHasKey;
}

void AOpenCharacter::GetKey()
{
	bHasKey = true;
}

int32 AOpenCharacter::GetXP_Implementation() const
{
	const AOpenPlayerState* OpenPlayerState = GetPlayerState<AOpenPlayerState>();
	check(OpenPlayerState);
	return OpenPlayerState->GetPlayerXP();
}

void AOpenCharacter::AddToXP_Implementation(int32 InXP)
{
	AOpenPlayerState* OpenPlayerState = GetPlayerState<AOpenPlayerState>();
	check(OpenPlayerState);
	OpenPlayerState->AddToXP(InXP);
}

int32 AOpenCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	const AOpenPlayerState* OpenPlayerState = GetPlayerState<AOpenPlayerState>();
	check(OpenPlayerState);
	return OpenPlayerState->LevelUpInfo->FindLevelForXP(InXP);
}

void AOpenCharacter::LevelUp_Implementation()
{
	MulticastLevelUpNiagara();
}

int32 AOpenCharacter::GetAttributePoint_Implementation() const
{
	const AOpenPlayerState* OpenPlayerState = GetPlayerState<AOpenPlayerState>();
	check(OpenPlayerState);
	return OpenPlayerState->GetAttributePoint();
}

int32 AOpenCharacter::GetSpellPoint_Implementation() const
{
	const AOpenPlayerState* OpenPlayerState = GetPlayerState<AOpenPlayerState>();
	check(OpenPlayerState);
	return OpenPlayerState->GetSpellPoint();
}

void AOpenCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	AOpenPlayerState* OpenPlayerState = GetPlayerState<AOpenPlayerState>();
	check(OpenPlayerState);
	OpenPlayerState->AddToLevel(InPlayerLevel);
}

int32 AOpenCharacter::GetAttributePointReward_Implementation(int32 InLevel) const
{
	const AOpenPlayerState* OpenPlayerState = GetPlayerState<AOpenPlayerState>();
	check(OpenPlayerState);
	return OpenPlayerState->LevelUpInfo->LevelUpInformation[InLevel].AttributePointAward;
}

int32 AOpenCharacter::GetSpellPointReward_Implementation(int32 InLevel) const
{
	const AOpenPlayerState* OpenPlayerState = GetPlayerState<AOpenPlayerState>();
	check(OpenPlayerState);
	return OpenPlayerState->LevelUpInfo->LevelUpInformation[InLevel].SpellPointAward;
}

void AOpenCharacter::AddToAttributePoints_Implementation(int32 InAttributePointsNum)
{
	AOpenPlayerState* OpenPlayerState = GetPlayerState<AOpenPlayerState>();
	check(OpenPlayerState);
	OpenPlayerState->AddToAttributePoints(InAttributePointsNum);
}

void AOpenCharacter::AddToSpellPoints_Implementation(int32 InSpellPointsNum)
{
	AOpenPlayerState* OpenPlayerState = GetPlayerState<AOpenPlayerState>();
	check(OpenPlayerState);
	OpenPlayerState->AddToSpellPoints(InSpellPointsNum);
}

UAnimMontage* AOpenCharacter::GetSpellMontage()
{
	return SpellMontage;
}

UAnimMontage* AOpenCharacter::GetRitualMontage()
{
	return RitualMontage;
}

FName AOpenCharacter::GetCharacterStateSection()
{
	if (CharacterState == ECharacterState::ECS_EquippedBothHands)
	{
		return FName("Both");
	}
	return FName("One");
}

bool AOpenCharacter::CanMeleeAttack()
{
	if (CharacterState == ECharacterState::ECS_EquippedBothHands || CharacterState == ECharacterState::ECS_EquippedRightHand)
	{
		return true;
	}
	return false;
}


