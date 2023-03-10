// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "InputAction.h"
#include "SpeedVehiclePawn.generated.h"

UENUM(BlueprintType)
enum class CameraStatus : uint8
{
	Follow UMETA(DisplayName = "Follow"),
	Manual UMETA(DisplayName = "Manual"),
	FirstPerson UMETA(DisplayName = "FirstPerson"),
	InMenu UMETA(DisplayName = "InMenu"),
	LookBack UMETA(DisplayName = "LookBack"),
	SnapToFollow UMETA(DisplayName = "SnapToFollow")
};

UENUM(BlueprintType)
enum class BombStatus : uint8
{
	None UMETA(DisplayName = "None"),
	Inactive UMETA(DisplayName = "Inactive"),
	Active UMETA(DisplayName = "Active"),
	Explodeded UMETA(DisplayName = "Explodeded")
};


UENUM(BlueprintType)
enum class InputType : uint8
{
	Keyboard UMETA(DisplayName = "Keyboard"),
	Controller UMETA(DisplayName = "Controller")
};

// TODO: Make base class and base blueprint to handle overlapping/duplicate code
UCLASS()
class SPEEDGAME_API ASpeedVehiclePawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:
	ASpeedVehiclePawn();	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Speedgame")
	void Init();

	void HandleVehicleGoingOffroad(bool isOnTrialTrack = false);

protected:
	virtual void PawnClientRestart() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Controls| Camera")
	class USpringArmComponent* CameraArmComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Controls| Camera")
	class UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* DeathParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Mesh | Second SkeletalMesh Asset")
	class USkeletalMesh* SecondaryMesh;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Speedgame")
	void SwitchCameraStatusTo(CameraStatus newCameraStatus);

	UPROPERTY(BluePrintReadOnly)
	BombStatus CurrentBombStatus = BombStatus::None;

	UPROPERTY(BlueprintReadOnly)
	CameraStatus CurrentCameraStatus = CameraStatus::Follow;

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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Actions")
	UInputAction* CameraZoomAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Actions")
	UInputAction* LookBehindAction;

	// INPUT MAPPING //
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Controls|Input Mappings")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Controls|Input Mappings")
	int32 InputMappingPriority = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Controls")
	InputType CurrentInputType = InputType::Keyboard;

	// INPUT FUCNTIONS //
	void Steer(const FInputActionInstance& ActionInstance);
	void Throttle(const FInputActionInstance& ActionInstance);
	void Brake(const FInputActionInstance& ActionInstance);
	void HandBrake(const FInputActionInstance& ActionInstance);
	void Look(const FInputActionInstance& ActionInstance);
	void LookBehind(const FInputActionInstance& ActionInstance);
	void CameraDistance(const FInputActionInstance& ActionInstance);

#pragma endregion

private:
	void HandleVehicleSpeed();
	void HandleCamera(float deltaTime);
	void Death();
	int GameSpeed = 50;
	float TimePassedSinceCameraInput = 0.f;
	float TimerSnapToFollow = 0.f;
	FVector PlayerStartLocation;
	FRotator PlayerStartRotation;
};
