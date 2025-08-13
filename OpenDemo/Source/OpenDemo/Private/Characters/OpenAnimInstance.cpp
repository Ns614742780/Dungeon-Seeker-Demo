#include "Characters/OpenAnimInstance.h"
#include "Characters/OpenCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


void UOpenAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OpenCharacter = Cast<AOpenCharacter>(TryGetPawnOwner());
	if (OpenCharacter)
	{
		OpenCharacterMovement = OpenCharacter->GetCharacterMovement();

	}
}

void UOpenAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (OpenCharacterMovement) {
		GroundSpeed = UKismetMathLibrary::VSizeXY(OpenCharacterMovement->Velocity);
		AirSpeed = OpenCharacterMovement->Velocity.Z;
		IsFalling = OpenCharacterMovement->IsFalling();
		CharacterState = OpenCharacter->GetCharacterState(); // ��ȡ��ɫ״̬
		ActionState = OpenCharacter->GetActionState(); // ��ȡ����״̬
		DeathPose = OpenCharacter->GetDeathPose(); // ��ȡ��������
	}
}
