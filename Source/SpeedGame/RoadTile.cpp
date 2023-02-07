// Fill out your copyright notice in the Description page of Project Settings.

#include "RoadTile.h"
#include "SpeedVehiclePawn.h"
#include "AIWheeledVehiclePawn.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include <Components/SphereComponent.h>
#include "Components/SplineComponent.h"
#include "RoadGenerator.h"
#include "SpeedGameGameModeBase.h"


ARoadTile::ARoadTile()
{
	PrimaryActorTick.bCanEverTick = true;

	CustomRootComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RoadMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RoadMesh"));
	NextRoadPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("NextRoadPoint"));
	SpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnPoint"));
	SpawnPointOncomming = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnPointOncomming"));

	// Road splines
	ForwardRightLane = CreateDefaultSubobject<USplineComponent>("ForwardRightLaneSpline");
	ForwardLefttLane = CreateDefaultSubobject<USplineComponent>("ForwardLefttLaneSpline");
	OncommingLeftLane = CreateDefaultSubobject<USplineComponent>("OncommingLeftLaneSpline");
	OncommingRightLane = CreateDefaultSubobject<USplineComponent>("OncommingRightLaneSpline");

	// Trigger Boxes
	ForwardTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ForwardTriggerBox"));
	OncommingTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OncommingTriggerBox"));
	LeftSideTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftSideTriggerBox"));
	RightSideTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightSideTriggerBox"));

	ForwardTriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	OncommingTriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	LeftSideTriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	RightSideTriggerBox->SetCollisionProfileName(TEXT("Trigger"));

	RootComponent = CustomRootComponent;

	RoadMeshComponent->SetupAttachment(RootComponent);
	NextRoadPoint->SetupAttachment(RootComponent);
	SpawnPoint->SetupAttachment(RootComponent);
	SpawnPointOncomming->SetupAttachment(RootComponent);

	// Trigger boxes
	ForwardTriggerBox->SetupAttachment(RootComponent);
	OncommingTriggerBox->SetupAttachment(RootComponent);
	LeftSideTriggerBox->SetupAttachment(RootComponent);
	RightSideTriggerBox->SetupAttachment(RootComponent);

	// Road Splines
	ForwardRightLane->SetupAttachment(RootComponent);
	ForwardLefttLane->SetupAttachment(RootComponent);
	OncommingLeftLane->SetupAttachment(RootComponent);
	OncommingRightLane->SetupAttachment(RootComponent);
}

void ARoadTile::Init(URoadGenerator* roadGenerator)
{
	RoadGenerator = roadGenerator;
}

void ARoadTile::BeginPlay()
{
	Super::BeginPlay();
	if (RoadTileType == RoadTileType::None)
		UE_LOG(LogTemp, Display, TEXT("RoadTyleType is not set in RoadTile blueprint"));

	ForwardTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ARoadTile::OnOverlapForwardBegin);
	OncommingTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ARoadTile::OnOverlapOncomingBegin);

	LeftSideTriggerBox->OnComponentEndOverlap.AddDynamic(this, &ARoadTile::OnSideTriggerBoxOverlapEnd);
	RightSideTriggerBox->OnComponentEndOverlap.AddDynamic(this, &ARoadTile::OnSideTriggerBoxOverlapEnd);
}


void ARoadTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


FAttachPointData ARoadTile::GetAttachPointData()
{
	return FAttachPointData(NextRoadPoint->GetComponentLocation(), NextRoadPoint->GetComponentQuat().Rotator());
}

RoadTileType ARoadTile::GetRoadTileType()
{
	return RoadTileType;
}


UArrowComponent* ARoadTile::GetForwardSpawnPoint()
{
	return SpawnPoint;
}

UArrowComponent* ARoadTile::GetOncommingSpawnPoint()
{
	return SpawnPointOncomming;
}

void ARoadTile::OnOverlapForwardBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AAIWheeledVehiclePawn* aiVehicle = Cast<AAIWheeledVehiclePawn>(OtherActor))
	{
		if (RoadGenerator && RoadGenerator->SpawnOneCarDebug && !IsTrialtrack)
			GenerateAndDestroyRoad();

		SetCurrentRoadTileForVehicleOrDestroy(NextTile, aiVehicle, LaneStatus::ForwardLeft, LaneStatus::ForwardRight);
	}

	if (ASpeedVehiclePawn* bus = Cast<ASpeedVehiclePawn>(OtherActor))
		GenerateAndDestroyRoad();
}

void ARoadTile::OnOverlapOncomingBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AAIWheeledVehiclePawn* aiVehicle = Cast<AAIWheeledVehiclePawn>(OtherActor))
		SetCurrentRoadTileForVehicleOrDestroy(PerviousTile, aiVehicle, LaneStatus::OncomingLeft, LaneStatus::OncomingRight);
}

void ARoadTile::OnSideTriggerBoxOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AAIWheeledVehiclePawn* aiVehicle = Cast<AAIWheeledVehiclePawn>(OtherActor))
		aiVehicle->CurrentCarStatus = CarStatus::Dying;
	if (ASpeedVehiclePawn* bus = Cast<ASpeedVehiclePawn>(OtherActor))
		bus->HandleVehicleGoingOffroad(IsTrialtrack);
}

void ARoadTile::SetCurrentRoadTileForVehicleOrDestroy(ARoadTile* roadTileToSet, AAIWheeledVehiclePawn* aiVehicle, LaneStatus LeftLaneStatus, LaneStatus RightLaneStatus)
{
	if (!roadTileToSet) //end of the road
	{
		aiVehicle->CurrentCarStatus = CarStatus::Dying;
		aiVehicle->SetCurrentRoad(nullptr);
		return;
	}

	if (aiVehicle->CurrentLane == LeftLaneStatus || aiVehicle->CurrentLane == RightLaneStatus)
		aiVehicle->SetCurrentRoad(roadTileToSet);
}


void ARoadTile::GenerateAndDestroyRoad()
{
	if (IsTrialtrack)
		return;

	if (RoadGenerator)
		RoadGenerator->AddRoadTile();

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			Destroy();
		}, 20, false);
}
