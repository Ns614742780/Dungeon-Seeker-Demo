#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Characters/CharacterTypes.h"
#include "OpenAnimInstance.generated.h"

class AOpenCharacter;
class UCharacterMovementComponent;

UCLASS()
class OPENDEMO_API UOpenAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly);
	AOpenCharacter* OpenCharacter;

	UPROPERTY(BlueprintReadOnly);
	UCharacterMovementComponent* OpenCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = Movement);
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement);
	float AirSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement);
	bool IsFalling;

	UPROPERTY(BlueprintReadOnly, Category = Movement);
	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadOnly, Category = Movement);
	EActionState ActionState;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	TEnumAsByte<EDeathPose> DeathPose;
};
