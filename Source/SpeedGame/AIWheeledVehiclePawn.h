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
};

UCLASS()
class SPEEDGAME_API AAIWheeledVehiclePawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()

	AAIWheeledVehiclePawn();

	UPROPERTY(EditAnywhere, Category = "SpeedGame | speed")
	float MaxSpeed = 50.f;

public:
	UPROPERTY(EditAnywhere)
	class USpringArmComponent* SrpingArmComp;

	UPROPERTY(EditAnywhere)
	class USceneComponent* LeftPoint;

	UPROPERTY(EditAnywhere)
	class USceneComponent* RightPoint;

	void SetCurrentRoad(class ARoadTile* CurrentRoadTile);

	LaneStatus CurrentLane;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpeedGame | speed")
	CarType CurrentCarType;

	void HandleVehicleGoingOffroad();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaSeconds) override;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* DeathParticle;

	void DriveInLane(float deltaSeconds);
	void SlowdownBehindVehicleAndChangeLane(float deltaSeconds);

	float TargetSplineDistance = 0.f;
	float CheckGap = 500.f;
	float SteerAmount = 0.f;
	float BrakeAmount = 0.f;

	UPROPERTY(EditAnywhere, Category = "SpeedGame | speed")
	TArray<float> VariableMaxSpeeds = { 45.f, 50.f, 60.f};

	class ARoadTile* CurrentRoadTile = nullptr;

	UFUNCTION(BlueprintImplementableEvent, Category = "SpeedGame | speed")
	void ChangeColor();

private:
	class USplineComponent* GetCurrentaneSpline();

	bool HasrecentlySwitchedLanes = false;
	bool isAlive = true;

	void SwitchLane();
	bool IsOtherCarOnOtherSideOfTheRoad(class AAIWheeledVehiclePawn* aiCar);

	void GetRandomCarTypeAndSetSpeed();
};
