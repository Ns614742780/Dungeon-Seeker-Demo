#include "Characters/BaseCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/OpenAbilitySystemComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);

	MeshComponent = GetMesh();
	MeshComponent->SetGenerateOverlapEvents(true);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
}

bool ABaseCharacter::IsItem_Implementation()
{
	return false;
}

FVector ABaseCharacter::GetCombatSocketLocation()
{
	check(MeshComponent);
	return MeshComponent->GetSocketLocation(SpellSocketName);
}

UAnimMontage* ABaseCharacter::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

FName ABaseCharacter::GetHitReactSectionName_Implementation(FVector HitPoint)
{
	const FVector ForwardVector = GetActorForwardVector();
	const FVector ToHit = (HitPoint - GetActorLocation()).GetSafeNormal();

	const double CosTheta = FVector::DotProduct(ForwardVector, ToHit);

	double Theta = FMath::Acos(CosTheta);
	Theta = FMath::RadiansToDegrees(Theta);

	// Determine the hit direction based on the angle
	const FVector CrossProduct = FVector::CrossProduct(ForwardVector, ToHit);

	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f; // Adjust the angle to be in the range [0, 360)
	}

	FName Section("FromBack");

	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}
	return Section;
}

UAnimMontage* ABaseCharacter::GetAttackMontage_Implementation()
{
	return AttackMontage;
}

int32 ABaseCharacter::GetAttackSectionCount_Implementation()
{
	return AttackMontageSections.Num();
}

int32 ABaseCharacter::GetMinionCount_Implementation()
{
	return MinionsCount;
}

void ABaseCharacter::AddMinionCount_Implementation(int32 Amount)
{
	MinionsCount += Amount;
}

bool ABaseCharacter::IsDead_Implementation() const
{
	return bIsDead;
}

AActor* ABaseCharacter::GetAvatar_Implementation()
{
	return this;
}

ECharacterClass ABaseCharacter::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

void ABaseCharacter::Die()
{
	if (Weapons.Num() > 0)
	{
		Weapons[ActiveWeaponIndex]->GetMesh()->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	}
	MulticastHandleDeath();
}

void ABaseCharacter::InitAbilityActorInfo() { }

void ABaseCharacter::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& GameplayEffectClass, const float Level) const
{
	check(GameplayEffectClass);
	check(IsValid(GetAbilitySystemComponent()));

	FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	
	const FGameplayEffectSpecHandle EffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
		GameplayEffectClass, Level, EffectContextHandle
	);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void ABaseCharacter::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.0f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.0f);
	ApplyEffectToSelf(DefaultMainAttributes, 1.0f);
}

void ABaseCharacter::AddCharacterAbilities()
{
	UOpenAbilitySystemComponent* OpenASC = CastChecked<UOpenAbilitySystemComponent>(AbilitySystemComponent);

	// 判断是否是服务器，如果不是服务器则直接返回
	if (!HasAuthority()) { return; }

	OpenASC->AddCharacterAbilities(StartupAbilities);
	OpenASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}

void ABaseCharacter::MulticastHandleDeath_Implementation()
{
	if (Weapons.Num() > 0)
	{
		Weapons[ActiveWeaponIndex]->GetMesh()->SetSimulatePhysics(true);
		Weapons[ActiveWeaponIndex]->GetMesh()->SetEnableGravity(true);
		Weapons[ActiveWeaponIndex]->GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}

	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetEnableGravity(true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	MeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bIsDead = true;
}

bool ABaseCharacter::CanAttack() const
{
	return false;
}

void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
}

void ABaseCharacter::AttackEnd() { }

bool ABaseCharacter::IsAlive() const
{
	return false;
}

void ABaseCharacter::DisableCapsuleCollision()
{
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
}

void ABaseCharacter::GetWeapon(AWeapon* Weapon)
{
	if (Weapon == nullptr || Weapons.Contains(Weapon))
	{
		return; // If the weapon is null or already in the list, do nothing
	}
	Weapons.Add(Weapon); // Add the weapon to the list of weapons
	++WeaponCount;
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector ForwardVector = GetActorForwardVector();
	const FVector ToHit = (ImpactPoint - GetActorLocation()).GetSafeNormal();

	const double CosTheta = FVector::DotProduct(ForwardVector, ToHit);

	double Theta = FMath::Acos(CosTheta);
	Theta = FMath::RadiansToDegrees(Theta);

	// Determine the hit direction based on the angle
	const FVector CrossProduct = FVector::CrossProduct(ForwardVector, ToHit);

	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f; // Adjust the angle to be in the range [0, 360)
	}

	FName Section("FromBack");

	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}

	PlayHitReactMontage(Section); // Play the hit reaction montage
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			ImpactPoint
		);
	}
}

void ABaseCharacter::PlayHitParticles(const FVector& ImpactPoint)
{
	if (HitParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticles,
			ImpactPoint
		);
	}
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{

}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::PlayAttackMontage()
{
	if (AttackMontageSections.Num() <= 0)
	{
		return;
	}

	const int32 SectionIndex = FMath::RandRange(0, AttackMontageSections.Num() - 1); // Randomly choose a section index from the attack montage sections
	const FName SectionName = AttackMontageSections[SectionIndex]; // Get the section name from the attack montage sections array
	PlayMontageSection(AttackMontage, SectionName); // Play the attack montage section
}

void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Stop(0.25f, AttackMontage); // Stop the attack montage with a fade-out time of 0.2 seconds
	}
}

int32 ABaseCharacter::PlayDeathMontage()
{
	if (DeathMontageSections.Num() <= 0)
	{
		return -1;
	}
	const int32 SectionIndex = FMath::RandRange(0, DeathMontageSections.Num() - 1); // Randomly choose a section index from the death montage sections
	const FName SectionName = DeathMontageSections[SectionIndex]; // Get the section name from the death montage sections array
	PlayMontageSection(DeathMontage, SectionName); // Play the death montage section

	if (SectionIndex < 0)
	{
		return SectionIndex; // If the selection is invalid, return immediately
	}

	TEnumAsByte<EDeathPose> DeadPose(SectionIndex); // Convert the selection index to an EDeathPose enum value
	if (DeadPose < EDeathPose::EDP_MAX)
	{
		DeathPose = DeadPose; // Set the death pose based on the selected montage section
	}
	
	return SectionIndex; // Return the section index for further use if needed
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget == nullptr)
	{
		return FVector();
	}

	const FVector CombatTargetLocation = CombatTarget->GetActorLocation(); // Get the location of the combat target
	const FVector CharacterLocation = GetActorLocation(); // Get the character's current location

	const FVector TargetDirection = (CharacterLocation - CombatTargetLocation).GetSafeNormal(); // Calculate the direction to the combat target
	const FVector WarpTarget = CombatTargetLocation + TargetDirection * WarpTargetDistance; // Calculate the warp target location based on the combat target's location and the warp distance

	return WarpTarget;
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation(); // Return the location of the combat target for rotation warping
	}
	return FVector();
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* AnimMontage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AnimMontage)
	{
		AnimInstance->Montage_Play(AnimMontage); // Play the specified animation montage
		AnimInstance->Montage_JumpToSection(SectionName, AnimMontage); // Jump to the specified section of the montage
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* ABaseCharacter::GetAttributeSet() const
{
	return AttributeSet;
}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (Weapons.Num() <= 0)
	{
		return; // If no weapons are available, do nothing
	}
	if (Weapons[ActiveWeaponIndex] && Weapons[ActiveWeaponIndex]->GetHitBox())
	{
		Weapons[ActiveWeaponIndex]->GetHitBox()->SetCollisionEnabled(CollisionEnabled); // Enable the hit box collision for the equipped weapon
		Weapons[ActiveWeaponIndex]->IgnoredActors.Empty(); // Clear the list of ignored actors in the weapon
	}
}