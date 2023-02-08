// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "AIWheeledVehiclePawn.generated.h"

UENUM()
enum class LaneStatus : uint8
{
	ForwardRight UMETA(DisplayName = "ForwardRight"),
	ForwardLeft UMETA(DisplayName = "ForwardLeft"),
	OncomingRight UMETA(DisplayName = "OncomingRight"),
	OncomingLeft UMETA(DisplayName = "OncomingLeft")
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
	UPROPERTY(EditAnywhere)
	class USceneComponent* LeftPoint;

	UPROPERTY(EditAnywhere)
	class USceneComponent* RightPoint;

	void SetCurrentRoad(class ARoadTile* CurrentRoadTile);

	LaneStatus CurrentLane;

	UPROPERTY(BluePrintReadOnly)
	CarStatus CurrentCarStatus = CarStatus::Alive;

	UPROPERTY(BlueprintReadOnly, Category = "SpeedGame | Car")
	CarType CurrentCarType;

	UPROPERTY(BlueprintReadOnly, Category = "SpeedGame | Car")
	float MaxSpeed = 50.f;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpeedGame | Car")
	bool IsParkedCar = false;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* DeathParticle;

	void DriveInLane(float deltaSeconds);
	void SlowdownBehindVehicleAndChangeLane(float deltaSeconds);

	float TargetSplineDistance = 0.f;
	float CheckGap = 400.f;
	float SteerAmount = 0.f;
	float BrakeAmount = 0.f;

	UPROPERTY(EditAnywhere, Category = "SpeedGame | Car")
	TArray<float> VariableMaxSpeeds = { 40.f, 50.f, 60.f};

	class ARoadTile* CurrentRoadTile = nullptr;

	UFUNCTION(BlueprintImplementableEvent, Category = "SpeedGame | Car")
	void ChangeColor();

private:
	class USplineComponent* GetCurrentaneSpline();
	
	FVector StartingLocation;
	bool HasrecentlySwitchedLanes = false;
	bool StartedDying = false;
	float DeathTimer = rand() % 7 + 5;
	float CurrenDeathTimer = 0;

	void SwitchLane();
	bool IsOtherCarOnOtherSideOfTheRoad(class AAIWheeledVehiclePawn* aiCar);

	void GetRandomCarTypeAndSetSpeed();
	void HandleVehicleGoingOffroad(float deltaSeconds);
};
