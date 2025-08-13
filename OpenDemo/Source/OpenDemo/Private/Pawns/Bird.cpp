// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/Bird.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ABird::ABird()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetCapsuleHalfHeight(20.0f);
	Capsule->SetCapsuleRadius(15.0f);

	SetRootComponent(Capsule);

	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	BirdMesh->SetupAttachment(RootComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f; // 设置相机距离

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	AutoPossessPlayer = EAutoReceiveInput::Player0; // 自动接管玩家输入
	
	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
	FloatingPawnMovement->SetUpdatedComponent(RootComponent);

	bUseControllerRotationPitch = true; // 允许控制器旋转俯仰
	bUseControllerRotationYaw = true; // 允许控制器旋转偏航

}

// Called when the game starts or when spawned
void ABird::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABird::handleMoveForward(float value)
{
	if (Controller && value != 0.f) {
		FVector ForwardDirection = GetActorForwardVector();
		AddMovementInput(ForwardDirection, value);
	}

}

void ABird::handleTurn(float value)
{
	AddControllerYawInput(value);
}

void ABird::handleLookUp(float value)
{
	AddControllerPitchInput(value);
}

// Called every frame
void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ABird::handleMoveForward);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ABird::handleTurn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ABird::handleLookUp);

}

