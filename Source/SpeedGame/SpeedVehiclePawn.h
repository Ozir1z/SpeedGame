// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "InputAction.h"
#include "SpeedVehiclePawn.generated.h"

UENUM()
enum class CameraStatus : uint8
{
	Follow UMETA(DisplayName = "Follow"),
	Manual UMETA(DisplayName = "Manual")
};

UENUM()
enum class BombStatus : uint8
{
	Inactive UMETA(DisplayName = "Inactive"),
	Active UMETA(DisplayName = "Active"),
	Explodeded UMETA(DisplayName = "Explodeded")
};

UCLASS()
class SPEEDGAME_API ASpeedVehiclePawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:
	ASpeedVehiclePawn();	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

protected:
	virtual void PawnClientRestart() override;

	UPROPERTY(EditAnywhere)
	class USpringArmComponent* CameraArmComp;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* DeathParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Mesh | Second SkeletalMesh Asset")
	class USkeletalMesh* SecondaryMesh;

#pragma region INPUT
	// INPUT ACTIONS //
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Controls|Input Actions")
	UInputAction* SteerAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Controls|Input Actions")
	UInputAction* ThrottleAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Controls|Input Actions")
	UInputAction* BrakeAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Controls|Input Actions")
	UInputAction* HandBrakeAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Actions")
	UInputAction* LookAction;

	// INPUT MAPPING //
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Controls|Input Mappings")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Controls|Input Mappings")
	int32 InputMappingPriority = 0;

	// INPUT FUCNTIONS //
	void Steer(const FInputActionInstance& ActionInstance);
	void Throttle(const FInputActionInstance& ActionInstance);
	void Brake(const FInputActionInstance& ActionInstance);
	void HandBrake(const FInputActionInstance& ActionInstance);
	void Look(const FInputActionInstance& ActionInstance);

#pragma endregion

private:
	UPROPERTY()
	CameraStatus CurrentCameraStatus = CameraStatus::Follow;
	
	UPROPERTY()
	BombStatus CurrentBombStatus = BombStatus::Inactive;

	void HandleVehicleSpeed();
	void Death();
	void SwitchCameraStatusTo(CameraStatus newCameraStatus);

	float TimePassedSinceCameraInput = 0.f;
};
