// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"

#include "OpenGameplayTags.h"
#include "AbilitySystem/OpenAbilitySystemComponent.h"
#include "AbilitySystem/OpenAttributeSet.h"
#include "UI/Widget/OpenUserWidget.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"

#include "Items/Weapons/Weapon.h"
#include "Components/AttributeComponent.h"
#include "UI/HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "AbilitySystem/OpenAbilitySystemLibrary.h"
#include "AI/OpenAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// 因为 enemy 无需使用 player state 管理自身的能力系统和属性集, 可以直接设置
	AbilitySystemComponent = CreateDefaultSubobject<UOpenAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UOpenAttributeSet>(TEXT("AttributeSet"));

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBar->SetupAttachment(RootComponent);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SetPeripheralVisionAngle(45.f);
	PawnSensing->SightRadius = 4000.f;
}

void AEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// AI only matters on the server
	if (!HasAuthority()) { return; }
	OpenAIController = Cast<AOpenAIController>(NewController);

	OpenAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	OpenAIController->RunBehaviorTree(BehaviorTree);

	// Set default value for blackboard
	OpenAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	OpenAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass == ECharacterClass::Player);
}

void AEnemy::BeginPlay()
{
	HandleLevelSet();
	Super::BeginPlay();
	
	// 因为 enemy 无需使用 player state 管理自身的能力系统和属性集,因此可以直接在 begin play 中初始化
	InitAbilityActorInfo();

	if (HasAuthority())
	{
		// 将数据资产中附加的能力添加到敌人的能力系统,只应该在服务器上进行
		UOpenAbilitySystemLibrary::GiveStartupAbilities(this, CharacterClass,AbilitySystemComponent);
	}

	if (UOpenUserWidget* OpenUserWidget = Cast<UOpenUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		OpenUserWidget->SetWidgetController(this);
	}

	if (const UOpenAttributeSet* OpenAS = Cast<UOpenAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(OpenAS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(OpenAS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);
		
		AbilitySystemComponent->RegisterGameplayTagEvent(FOpenGameplayTags::Get().Ability_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&AEnemy::HitReactTagChanged
		);

		OnHealthChanged.Broadcast(OpenAS->GetHealth());
		OnMaxHealthChanged.Broadcast(OpenAS->GetMaxHealth());
	}

	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget); // Move to the initial patrol target if it exists
	
	SpawnDefaultWeapon();

	if (bIsPlayerFriend)
	{
		Tags.Add(FName("Player"));
	}
	else
	{
		Tags.Add(FName("Enemy")); // Add a tag to the enemy for identification
	}
}
void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		GetWeapon(DefaultWeapon);
		DefaultWeapon->EquipWeapon(GetMesh(), DefaultWeapon->GetWeaponSocketName(), this, this);
		ActiveWeaponIndex = 0;
	}
}
// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*
	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget(); // Check if the enemy should patrol to a new target
	}
	*/
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn(); // Set the combat target to this enemy
	
	if (IsInSideAttackRadius())
	{
		EnemyState = EEnemyState::EES_Attacking;
	}

	return DamageAmount;
}

void AEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	
	Cast<UOpenAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	// 只应该在服务器上初始化
	if (HasAuthority())
	{
		InitializeDefaultAttributes();
	}
}

void AEnemy::InitializeDefaultAttributes() const
{
	UOpenAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, CharacterLevel, AbilitySystemComponent);
}

void AEnemy::Die()
{
	SetLifeSpan(DeathLifeSpan);
	if (OpenAIController)
	{
		OpenAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
	}
	EnemyState = EEnemyState::EES_Dead; // Change the enemy state to dead
	GetCharacterMovement()->bOrientRotationToMovement = false; // Disable character movement orientation to prevent further movement after death
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision); // Disable weapon collision to prevent further interactions

	PlayDeathMontage();

	Super::Die();
}

void AEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AEnemy::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

void AEnemy::SetCombatSpeed_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AEnemy::SetPatrolSpeed_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AEnemy::SetEnemyLevel(int32 InLevel)
{
	if (CharacterLevel != InLevel)
	{
		CharacterLevel = InLevel;

		if (HasActorBegunPlay())
		{
			HandleLevelSet();
		}
	}
}

bool AEnemy::InTargetRange(AActor* Target, double Radius) const
{
	if (Target == nullptr) // Check if the target is valid
	{
		return false; // If the target is null, return false
	}
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size(); // Calculate the distance to the target
	return DistanceToTarget <= Radius; // Return true if the target is within the specified radius
}
void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr)
	{
		return; // Ensure the enemy controller and target are valid before proceeding
	}

	FAIMoveRequest MoveRequest; // Create a move request for the AI controller
	MoveRequest.SetGoalActor(Target); // Set the goal actor to this enemy
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius); // Set the acceptance radius for the AI controller to stop moving when close enough to the target

	EnemyController->MoveTo(MoveRequest); // Set the focal point of the AI controller to the enemy's location
}
AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> NewPatrolTargets;

	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget) // Exclude the current patrol target from the list
		{
			NewPatrolTargets.AddUnique(Target); // Add the target to the new patrol targets array
		}
	}

	const int32 NumPatrolTargets = NewPatrolTargets.Num(); // Get the number of patrol targets

	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1); // Randomly choose a patrol target from the array
		return NewPatrolTargets[TargetSelection]; // Get the selected patrol target
	}

	return nullptr;
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget); // Move to the patrol target when the patrol timer finishes
}
void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer); // Clear the patrol timer if a player is seen
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr; // Clear the combat target if it's too far away
}
void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling; // Change the enemy state back to patrolling
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	MoveToTarget(PatrolTarget); // Move to the patrol target
}

bool AEnemy::IsOutSideCombatRadius() const
{
	return !InTargetRange(CombatTarget, CombatRadius);
}
bool AEnemy::IsOutSideAttackRadius() const
{
	return !InTargetRange(CombatTarget, AttackRadius);
}
bool AEnemy::IsInSideAttackRadius() const
{
	return InTargetRange(CombatTarget, AttackRadius);
}
bool AEnemy::IsChasing() const
{
	return EnemyState == EEnemyState::EES_Chasing;
}
bool AEnemy::IsAttacking() const
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsEngaged() const
{
	return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget(); // Get the selected patrol target

		const float WaitTime = FMath::RandRange(PatrolWaitMinTime, PatrolWaitMaxTime); // Randomly choose a wait time between the minimum and maximum wait times

		GetWorldTimerManager().SetTimer(
			PatrolTimer,
			this,
			&AEnemy::PatrolTimerFinished,
			WaitTime,
			false
		); // Set a timer to finish patrolling after the specified wait time
	}
}

// Called to bind functionality to input
void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter); // Call the base class GetHit function
}
int32 AEnemy::GetPlayerLevel_Implementation()
{
	return CharacterLevel;
}

void AEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : RunSpeed;
	if (OpenAIController && OpenAIController->GetBlackboardComponent())
	{
		OpenAIController->GetBlackboardComponent()->SetValueAsBool(FName("bHitReacting"), bHitReacting);
	}
}

void AEnemy::HandleLevelSet()
{
	InitializeDefaultAttributes();

	if (const UOpenAttributeSet* OpenAttributeSet = Cast<UOpenAttributeSet>(AttributeSet))
	{
		OnHealthChanged.Broadcast(OpenAttributeSet->GetMaxHealth());
		OnMaxHealthChanged.Broadcast(OpenAttributeSet->GetMaxHealth());
	}
}

void AEnemy::Destroyed()
{
	for (AWeapon* Weapon : Weapons)
	{
		Weapon->Destroy(); // Destroy all weapons when the enemy is destroyed
	}
}
