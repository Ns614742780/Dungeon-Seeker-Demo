// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Bird.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class UFloatingPawnMovement;

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class OPENDEMO_API ABird : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABird();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void handleMoveForward(float value);
	void handleTurn(float value);
	void handleLookUp(float value);

private:
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* Capsule;
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* BirdMesh;
	UPROPERTY(VisibleAnywhere)
	UFloatingPawnMovement* FloatingPawnMovement;
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

};
