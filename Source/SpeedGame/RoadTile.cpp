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

const FName ARoadTile::ForwardTriggerName = FName("ForwardTriggerBox");
const FName ARoadTile::OncommingTriggerName = FName("OncommingTriggerBox");
const FName ARoadTile::LeftSideTriggerName = FName("LeftSideTriggerBox");
const FName ARoadTile::RightSideTriggerName = FName("RightSideTriggerBox");

ARoadTile::ARoadTile()
{
	PrimaryActorTick.bCanEverTick = true;

	CustomRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
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
	ForwardTriggerBox = CreateDefaultSubobject<UBoxComponent>(ForwardTriggerName);
	OncommingTriggerBox = CreateDefaultSubobject<UBoxComponent>(OncommingTriggerName);
	LeftSideTriggerBox = CreateDefaultSubobject<UBoxComponent>(LeftSideTriggerName);
	RightSideTriggerBox = CreateDefaultSubobject<UBoxComponent>(RightSideTriggerName);

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

void ARoadTile::Init(URoadGenerator* roadGenerator, FLinearColor color)
{
	RoadGenerator = roadGenerator;
	DynamicMaterial_RoadMesh->SetVectorParameterValue(TEXT("LineColor"), color);
}

void ARoadTile::SpawnCar(TSubclassOf<class AAIWheeledVehiclePawn> aiCarBP)
{
	int forwardOrOncomming = rand() % 2; // 50 % forward 0, oncomming 1

	UArrowComponent* arrowDirection = forwardOrOncomming == 0 ? GetForwardSpawnPoint() : GetOncommingSpawnPoint();
	AAIWheeledVehiclePawn* aiVehicle = GetWorld()->SpawnActor<AAIWheeledVehiclePawn>(aiCarBP, arrowDirection->GetComponentLocation(), arrowDirection->GetComponentRotation());

	if (aiVehicle == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawning AI car failed"));
		return;
	}

	DriveDirection direction = forwardOrOncomming == 0 ? DriveDirection::Forward : DriveDirection::Oncomming;

	aiVehicle->Init(this, direction);
	AIVehicilesOnThisRoad.Add(aiVehicle);
}

void ARoadTile::BeginPlay()
{
	Super::BeginPlay();
	if (RoadTileType == RoadTileType::None)
		UE_LOG(LogTemp, Display, TEXT("RoadTyleType is not set in RoadTile blueprint"));

	ForwardTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ARoadTile::OnOverlapForwardBegin);

	Material = RoadMeshComponent->GetMaterial(0);
	DynamicMaterial_RoadMesh = UMaterialInstanceDynamic::Create(Material, this);
	RoadMeshComponent->SetMaterial(0, DynamicMaterial_RoadMesh);
}

void ARoadTile::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

	if (TimetoDestroy)
		DestroyTimer -= deltaSeconds;

	if (DestroyTimer <= 0 && TimetoDestroy)
	{
		TimetoDestroy = false; // only destroy once
		Destroy();
	}
}

void ARoadTile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
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
	if (ASpeedVehiclePawn* bus = Cast<ASpeedVehiclePawn>(OtherActor))
		GenerateAndDestroyRoad();
}



void ARoadTile::GenerateAndDestroyRoad()
{
	if (IsTrialtrack)
		return;

	if (RoadGenerator)
		RoadGenerator->AddRoadTile();

	TimetoDestroy = true;
}
