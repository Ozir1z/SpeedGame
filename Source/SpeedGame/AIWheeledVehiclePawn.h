// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "AIWheeledVehiclePawn.generated.h"

UENUM()
enum class LaneStatus
{
	ForwardRight UMETA(DisplayName = "ForwardRight"),
	ForwardLeft UMETA(DisplayName = "ForwardLeft"),
	OncomingRight UMETA(DisplayName = "OncomingRight"),
	OncomingLeft UMETA(DisplayName = "OncomingLeft")
};

UENUM()
enum class DriveDirection
{
	Forward UMETA(DisplayName = "Forward"),
	Oncomming UMETA(DisplayName = "Oncomming")
};

UENUM(BlueprintType)
enum class CarType : uint8 
{
	Slow = 0 UMETA(DisplayName = "Slow"),
	Normal = 1 UMETA(DisplayName = "Normal"),
	Fast = 2 UMETA(DisplayName = "Fast"),
	Insane =3 UMETA(DisplayName = "Insane"),
};

UENUM(BlueprintType)
enum class CarStatus : uint8
{
	Alive = 0 UMETA(DisplayName = "Alive"),
	Dying = 1 UMETA(DisplayName = "Dying"),
	Dead = 2 UMETA(DisplayName = "Dead"),
};


// TODO: Make base class and base blueprint to handle overlapping/duplicate code
UCLASS()
class SPEEDGAME_API AAIWheeledVehiclePawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()

	AAIWheeledVehiclePawn();

public:

	void Init(class ARoadTile* currentRoadTile, DriveDirection direction);
	void SetCurrentRoadTile(class ARoadTile* currentRoadTile);

	UPROPERTY(EditAnywhere)
	class USceneComponent* LeftPoint;

	UPROPERTY(EditAnywhere)
	class USceneComponent* RightPoint;

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly)
	CarStatus CurrentCarStatus = CarStatus::Alive;

	UPROPERTY(BlueprintReadOnly, Category = "SpeedGame | Car")
	CarType CurrentCarType;

	UPROPERTY(BlueprintReadOnly, Category = "SpeedGame | Car")
	float MaxSpeed = 50.f;

	DriveDirection CurrentDriveDirection;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float deltaSeconds) override;

	void DriveInLane(float deltaSeconds);
	void SlowdownBehindVehicleAndChangeLane(float deltaSeconds);

	UFUNCTION(BlueprintImplementableEvent, Category = "SpeedGame | Car")
	void ChangeColor();

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpeedGame | Car")
	bool IsParkedCar = false;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* DeathParticle;

	float TargetSplineDistance = 0.f;
	float CheckGap = 400.f;
	float SteerAmount = 0.f;
	float BrakeAmount = 0.f;

	UPROPERTY(EditAnywhere, Category = "SpeedGame | Car")
	TArray<float> VariableMaxSpeeds = { 40.f, 50.f, 60.f};

	class ARoadTile* CurrentRoadTile = nullptr;

private:
	class USplineComponent* GetCurrentaneSpline();

	LaneStatus CurrentLane;
	FVector StartingLocation;
	FTimerHandle SwitchLaneTimerHandle;

	bool HasrecentlySwitchedLanes = false;
	bool StartedDying = false;
	bool SpawnComplete = false;
	float DeathTimer = rand() % 7 + 5;
	float CurrenDeathTimer = 0;
	
	void SwitchLane();
	bool IsOtherCarOnOtherSideOfTheRoad(class AAIWheeledVehiclePawn* aiCar);

	void GetRandomCarTypeAndSetSpeed();
	void HandleVehicleGoingOffroad(float deltaSeconds);
};
