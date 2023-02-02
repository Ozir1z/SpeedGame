// Fill out your copyright notice in the Description page of Project Settings.


#include "AIWheeledVehiclePawn.h"
#include <ChaosVehicleMovementComponent.h>
#include "GameFramework/SpringArmComponent.h"
#include "Components/SplineComponent.h"
#include "Components/BoxComponent.h"
#include "RoadTile.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


AAIWheeledVehiclePawn::AAIWheeledVehiclePawn() 
{
	SrpingArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	LeftPoint = CreateDefaultSubobject<USceneComponent>(TEXT("LeftPoint"));
	RightPoint = CreateDefaultSubobject<USceneComponent>(TEXT("RightPoint"));

	SrpingArmComp->SetupAttachment(RootComponent);
	LeftPoint->SetupAttachment(RootComponent);
	RightPoint->SetupAttachment(RootComponent);

	SrpingArmComp->TargetArmLength = 0;
	SrpingArmComp->bDoCollisionTest = false;
}

void AAIWheeledVehiclePawn::BeginPlay()
{
	Super::BeginPlay();

	GetRandomCarTypeAndSetSpeed();
}

void AAIWheeledVehiclePawn::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

	if (CurrentRoadTile && GetActorLocation().Z < CurrentRoadTile->GetActorLocation().Z - 150)
		CurrentCarStatus = CarStatus::Dying;

	if (CurrentCarStatus == CarStatus::Dead)
		return; // this fool done

	
	if (CurrentRoadTile == nullptr || CurrentCarStatus == CarStatus::Dying)
	{
		HandleVehicleGoingOffroad(deltaSeconds);
		return; // we dont wanna do shit if there is no current road tile
	}

	SlowdownBehindVehicleAndChangeLane(deltaSeconds);
	DriveInLane(deltaSeconds);
}

void AAIWheeledVehiclePawn::HandleVehicleGoingOffroad(float deltaSeconds)
{
	CurrenDeathTimer += deltaSeconds;
	if (!StartedDying)
	{
		StartedDying = true;
		GetMesh()->SetEnableGravity(false);
		SetCurrentRoad(nullptr);
	}

	if (CurrenDeathTimer < DeathTimer)
		return;

	CurrentCarStatus = CarStatus::Dead;
	GetMesh()->SetEnableGravity(false);
	SetCurrentRoad(nullptr);

	if (DeathParticle)
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DeathParticle, GetActorLocation(), GetActorRotation(), FVector(5.0f));
	Destroy();
}

void AAIWheeledVehiclePawn::SetCurrentRoad(ARoadTile* currentRoadTile)
{
	CurrentRoadTile = currentRoadTile;
	TargetSplineDistance = 0.f;
}

void AAIWheeledVehiclePawn::DriveInLane(float deltaSeconds)
{
	float throttleInput = GetVehicleMovement()->GetForwardSpeedMPH() <= MaxSpeed ? 1 : 0;
	GetVehicleMovement()->SetThrottleInput(throttleInput);
	
	FVector targetPoint = GetCurrentaneSpline()->GetLocationAtDistanceAlongSpline(TargetSplineDistance, ESplineCoordinateSpace::World);
	double distance = FVector::Dist(targetPoint, GetActorLocation());

	if (distance < CheckGap)
		TargetSplineDistance += CheckGap;


	float leftPointDistance = FVector::Distance(targetPoint, LeftPoint->GetComponentLocation());
	float rightPointDistance = FVector::Distance(targetPoint, RightPoint->GetComponentLocation());
	float steeringInput = leftPointDistance > rightPointDistance ? .45f : -.45f;

	float spaceBetweenLeftandRightAbs = abs(leftPointDistance - rightPointDistance);

	float deadzone = 1.0f;
	if (spaceBetweenLeftandRightAbs < 25)
		deadzone = 0.75f;
	if (spaceBetweenLeftandRightAbs < 20)
		deadzone = 0.5f;
	if (spaceBetweenLeftandRightAbs < 10)
		deadzone = 0.25f;
	if (spaceBetweenLeftandRightAbs < 5)
		deadzone = 0.f;

	steeringInput *= deadzone;
	SteerAmount = FMath::Lerp(SteerAmount, steeringInput, deltaSeconds * 40);
	GetVehicleMovement()->SetSteeringInput(SteerAmount);
}

void AAIWheeledVehiclePawn::SlowdownBehindVehicleAndChangeLane(float deltaSeconds)
{
	//refactor this shit its terrifying
	FVector start = GetActorLocation();
	FVector forward = GetActorForwardVector();
	start = FVector(start.X + (forward.X * 50), start.Y, start.Z);

	FVector endNormalBrake = start + (forward * 1500); // range of raycast
	FVector endHandBrake = start + (forward * 3000); // range of raycast
	FHitResult hitNormalBrake;

	FHitResult hitHandBrake;
	FCollisionQueryParams CollisionParameters;
	CollisionParameters.AddIgnoredActor(this);

	bool actorHitNormalBrake = GetWorld()->LineTraceSingleByChannel(hitNormalBrake, start, endNormalBrake, ECC_Pawn, CollisionParameters, FCollisionResponseParams());
	//DrawDebugLine(GetWorld(), start, endNormalBrake, FColor::Green, false, 0.1f, 0.f, 10.f);
	
	bool actorHitHandBrake = GetWorld()->LineTraceSingleByChannel(hitHandBrake, start, endHandBrake, ECC_Pawn, CollisionParameters, FCollisionResponseParams());
	//DrawDebugLine(GetWorld(), start, endHandBrake, FColor::Red, false, 0.1f, 0.f, 10.f);
	float breakAmount;

	if (actorHitHandBrake && hitHandBrake.GetActor())
	{
		AAIWheeledVehiclePawn* aiCar = Cast<AAIWheeledVehiclePawn>(hitHandBrake.GetActor());
		if (aiCar)
		{
			if (IsOtherCarOnOtherSideOfTheRoad(aiCar))
				return;
		}

		AWheeledVehiclePawn* vehicle = Cast<AWheeledVehiclePawn>(hitHandBrake.GetActor());
		if (vehicle)
		{
			if (vehicle->GetVehicleMovement()->GetForwardSpeedMPH() <= 20)
			{
				GetVehicleMovement()->SetHandbrakeInput(true);
				breakAmount = 1.f;
				SwitchLane(); 
			}
		}
	}
	else GetVehicleMovement()->SetHandbrakeInput(false);

	if (actorHitNormalBrake && hitNormalBrake.GetActor())
	{
		AAIWheeledVehiclePawn* aiCar = Cast<AAIWheeledVehiclePawn>(hitHandBrake.GetActor());
		if (aiCar)
		{
			if (IsOtherCarOnOtherSideOfTheRoad(aiCar))
				return;

			SwitchLane();
		}

		AWheeledVehiclePawn* vehicle = Cast<AWheeledVehiclePawn>(hitNormalBrake.GetActor());
		
		if (vehicle) breakAmount = .5f;
	}
	else breakAmount = 0.f;

	BrakeAmount = FMath::Lerp(BrakeAmount, breakAmount, deltaSeconds * 40);
	GetVehicleMovement()->SetBrakeInput(BrakeAmount);
}

USplineComponent* AAIWheeledVehiclePawn::GetCurrentaneSpline()
{
	switch (CurrentLane)
	{
		case LaneStatus::ForwardLeft:
			return CurrentRoadTile->ForwardLefttLane;
			break;
		case LaneStatus::ForwardRight:
			return CurrentRoadTile->ForwardRightLane;
			break;
		case LaneStatus::OncomingLeft:
			return CurrentRoadTile->OncommingLeftLane;
			break;
		case LaneStatus::OncomingRight:
			return CurrentRoadTile->OncommingRightLane;
			break;
		default:
			return nullptr;
	}
}

void AAIWheeledVehiclePawn::SwitchLane()
{
	if (HasrecentlySwitchedLanes || 
		CurrentRoadTile->GetRoadTileType() == RoadTileType::CornerRight ||
		CurrentRoadTile->GetRoadTileType() == RoadTileType::CornerLeft)
		return; // dont switch if on CD or in a corner

	HasrecentlySwitchedLanes = true;
	SteerAmount = 0.1f;

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			HasrecentlySwitchedLanes = false;
		}, 5, false); // 5 seconds make variable?

	switch (CurrentLane)
	{
		case LaneStatus::ForwardLeft:
			CurrentLane = LaneStatus::ForwardRight;
			break;
		case LaneStatus::ForwardRight:
			CurrentLane = LaneStatus::ForwardLeft;
			break;
		case LaneStatus::OncomingLeft:
			CurrentLane = LaneStatus::OncomingRight;
			break;
		case LaneStatus::OncomingRight:
			CurrentLane = LaneStatus::OncomingLeft;
			break;
	}
}

bool AAIWheeledVehiclePawn::IsOtherCarOnOtherSideOfTheRoad(AAIWheeledVehiclePawn* aiCar)
{
	if ((CurrentLane == LaneStatus::ForwardRight || CurrentLane == LaneStatus::ForwardLeft) &&
		(aiCar->CurrentLane == LaneStatus::OncomingRight || aiCar->CurrentLane == LaneStatus::OncomingLeft))
		return true;
	else if ((CurrentLane == LaneStatus::OncomingRight || CurrentLane == LaneStatus::OncomingLeft) &&
		(aiCar->CurrentLane == LaneStatus::ForwardRight || aiCar->CurrentLane == LaneStatus::ForwardLeft))
		return true;
	else 
		return false;
}

void AAIWheeledVehiclePawn::GetRandomCarTypeAndSetSpeed()
{
	float random = rand() % VariableMaxSpeeds.Num();

	CurrentCarType = CarType(random);
	MaxSpeed = VariableMaxSpeeds[random];
	AAIWheeledVehiclePawn::ChangeColor();
}