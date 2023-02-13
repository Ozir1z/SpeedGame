// Fill out your copyright notice in the Description page of Project Settings.


#include "AIWheeledVehiclePawn.h"
#include <ChaosVehicleMovementComponent.h>
#include <ChaosWheeledVehicleMovementComponent.h>
#include "GameFramework/SpringArmComponent.h"
#include "Components/SplineComponent.h"
#include "Components/BoxComponent.h"
#include "RoadTile.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AAIWheeledVehiclePawn::AAIWheeledVehiclePawn() 
{
	PrimaryActorTick.bCanEverTick = true;
	LeftPoint = CreateDefaultSubobject<USceneComponent>(TEXT("LeftPoint"));
	RightPoint = CreateDefaultSubobject<USceneComponent>(TEXT("RightPoint"));

	LeftPoint->SetupAttachment(RootComponent);
	RightPoint->SetupAttachment(RootComponent);

}

void AAIWheeledVehiclePawn::BeginPlay()
{
	Super::BeginPlay();
	if (!GetMesh())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Mesh attached to AI car"));
		return;
	}

	StartingLocation = GetMesh()->GetComponentLocation();
	GetMesh()->OnComponentBeginOverlap.AddDynamic(this, &AAIWheeledVehiclePawn::OnOverlapBegin);
	GetRandomCarTypeAndSetSpeed();
}

void AAIWheeledVehiclePawn::Init(ARoadTile* currentRoadTile, DriveDirection direction)
{
	CurrentLane = direction == DriveDirection::Forward ? LaneStatus::ForwardRight : LaneStatus::OncomingRight;
	CurrentDriveDirection = direction;
	SetCurrentRoadTile(currentRoadTile);
	SpawnComplete = true;
}

void AAIWheeledVehiclePawn::GetRandomCarTypeAndSetSpeed()
{
	float random = rand() % VariableMaxSpeeds.Num();
	float insaneCarChance = rand() % 9;

	if (insaneCarChance == 0)
	{
		CurrentCarType = CarType::Insane;
		MaxSpeed = 75.f;
	}
	else {
		CurrentCarType = CarType(random);
		MaxSpeed = VariableMaxSpeeds[random];
	}

	AAIWheeledVehiclePawn::ChangeColor();
}


void AAIWheeledVehiclePawn::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);
	if (!SpawnComplete)
		return;

	if (IsParkedCar)
	{
		if(GetMesh()->GetComponentLocation().Z <= (StartingLocation.Z - 300))
			HandleVehicleGoingOffroad(deltaSeconds);
		return;
	}

	if (CurrentRoadTile && GetActorLocation().Z <= (CurrentRoadTile->GetActorLocation().Z - 300))
		CurrentCarStatus = CarStatus::Dying;


	if (CurrentCarStatus == CarStatus::Dead)
		return; // this fool done

	
	if (CurrentRoadTile == nullptr || CurrentCarStatus == CarStatus::Dying)
	{
		HandleVehicleGoingOffroad(deltaSeconds);
		return; // we dont wanna do shit if there is no current road tile or if we are dying
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
		SetCurrentRoadTile(nullptr);
	}

	if (CurrenDeathTimer < DeathTimer)
		return;

	CurrentCarStatus = CarStatus::Dead;

	if (DeathParticle)
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DeathParticle, GetActorLocation(), GetActorRotation(), FVector(5.0f));
	Destroy();
}

void AAIWheeledVehiclePawn::DriveInLane(float deltaSeconds)
{
	if (!GetCurrentaneSpline())
		return;

	float throttleInput = GetVehicleMovement()->GetForwardSpeedMPH() <= MaxSpeed ? 1 : 0;
	GetVehicleMovement()->SetThrottleInput(throttleInput);

	FVector targetPoint = GetCurrentaneSpline()->GetLocationAtDistanceAlongSpline(TargetSplineDistance, ESplineCoordinateSpace::World);


	double distance = FVector::Dist(targetPoint, GetMesh()->GetComponentLocation());

	if (distance < CheckGap)
		TargetSplineDistance += CheckGap;


	float leftPointDistance = FVector::Distance(targetPoint, LeftPoint->GetComponentLocation());
	float rightPointDistance = FVector::Distance(targetPoint, RightPoint->GetComponentLocation());
	float steerAmount = CurrentCarType == CarType::Insane ? .55f : .45f;

	float steeringInput = leftPointDistance > rightPointDistance ? steerAmount : -steerAmount;

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
	float breakAmount;
	FVector start = GetActorLocation();
	FVector forward = GetActorForwardVector();
	start = FVector(start.X + (forward.X * 50), start.Y, start.Z);

	FCollisionQueryParams CollisionParameters;
	CollisionParameters.AddIgnoredActor(this);

	// handle handbrake
	FVector endHandBrake = start + (forward * 1500); // range of raycast
	FHitResult hitHandBrake;
	bool actorHitHandBrake = GetWorld()->LineTraceSingleByChannel(hitHandBrake, start, endHandBrake, ECC_Pawn, CollisionParameters, FCollisionResponseParams());

	if (actorHitHandBrake && hitHandBrake.GetActor())
	{
		if (AWheeledVehiclePawn* vehicle = Cast<AWheeledVehiclePawn>(hitHandBrake.GetActor()))
		{
			GetVehicleMovement()->SetHandbrakeInput(true);
			breakAmount = 1.f;
		}

		AAIWheeledVehiclePawn* aiCar = Cast<AAIWheeledVehiclePawn>(hitHandBrake.GetActor());
		if (aiCar && !IsOtherCarOnOtherSideOfTheRoad(aiCar))
			SwitchLane();
	}
	else 
		GetVehicleMovement()->SetHandbrakeInput(false);

	// handle handbrake
	FVector endNormalBrake = start + (forward * 3000); // range of raycast
	FHitResult hitNormalBrake;
	bool actorHitNormalBrake = GetWorld()->LineTraceSingleByChannel(hitNormalBrake, start, endNormalBrake, ECC_Pawn, CollisionParameters, FCollisionResponseParams());

	if (actorHitNormalBrake && hitNormalBrake.GetActor())
	{
		AAIWheeledVehiclePawn* aiCar = Cast<AAIWheeledVehiclePawn>(hitNormalBrake.GetActor());
		if (aiCar && !IsOtherCarOnOtherSideOfTheRoad(aiCar))
			SwitchLane();

		if (AWheeledVehiclePawn* vehicle = Cast<AWheeledVehiclePawn>(hitNormalBrake.GetActor())) 
			breakAmount = .5f;
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
		CurrentRoadTile->GetRoadTileType() == RoadTileType::CornerLeft ||
		GetVehicleMovement()->GetForwardSpeedMPH() <= 10)
		return; // dont switch if on CD or in a turn

	HasrecentlySwitchedLanes = true;	
	SteerAmount = 0.1f;

	FTimerHandle TimerHandle;
	float randomTimeToSwitch = rand() % 20 + 10; // between 10-20 seconds
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			HasrecentlySwitchedLanes = false;
		}, randomTimeToSwitch, false); // 5 seconds make variable?

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

void AAIWheeledVehiclePawn::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!SpawnComplete)
		return;

	ARoadTile* roadTile = Cast<ARoadTile>(OtherActor);
	UBoxComponent * trigger = Cast<UBoxComponent>(OtherComp);
	if (roadTile && trigger)
	{
		if (trigger->GetName().Equals(ARoadTile::ForwardTriggerName.ToString())
			&& CurrentDriveDirection == DriveDirection::Forward)
		{
			SetCurrentRoadTile(roadTile->NextTile);
		}
		else if (trigger->GetName().Equals(ARoadTile::OncommingTriggerName.ToString())
			&& CurrentDriveDirection == DriveDirection::Oncomming)
		{
			SetCurrentRoadTile(roadTile->PreviousTile);
		}
		else if (trigger->GetName().Equals(ARoadTile::LeftSideTriggerName.ToString())
			|| trigger->GetName().Equals(ARoadTile::RightSideTriggerName.ToString()))
		{
			CurrentCarStatus = CarStatus::Dying; 
		}
	}
}

void AAIWheeledVehiclePawn::SetCurrentRoadTile(ARoadTile* roadTile)
{
	if (roadTile)
		CurrentRoadTile = roadTile;
	else
	{
		CurrentRoadTile = nullptr;
		CurrentCarStatus = CarStatus::Dying;
	}
}