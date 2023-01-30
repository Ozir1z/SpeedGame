// Fill out your copyright notice in the Description page of Project Settings.

#include "RoadGenerator.h"
#include "RoadTile.h"
#include "AIWheeledVehiclePawn.h"
#include "Components/ArrowComponent.h"

URoadGenerator::URoadGenerator()
{
	PrimaryComponentTick.bCanEverTick = true;

	FirstSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("FirstSpawnPoint"));
}

void URoadGenerator::BeginPlay()
{
	Super::BeginPlay();

	if (!RoadTileBPStraight || !RoadTileBPCornerLeft || !RoadTileBPCornerRight
		|| !RoadTileBPSlopeBot || !RoadTileBPSlopeTop || !AICarBP)
	{
		UE_LOG(LogTemp, Display, TEXT("AICarBP or RoadTileBPs are not set in RoadGenerator blueprint"));
		return;
	}

	if (IsDebugTrack)
	{
		GenerateDebugTrack();
		return;
	}

	if (!GenerateInitialRoad)
		return;

	if (SpawnOneCarDebug)
		SpawnOneCarDebugCompleted = false;

	NextSpawnPointData = FAttachPointData{
		FirstSpawnPoint->GetComponentLocation(),
		FirstSpawnPoint->GetComponentQuat().Rotator()
	};


	RoadTilesTopCollection.Add(RoadTileBPSlopeTop);
	RoadTilesTopCollection.Add(RoadTileBPStraight);

	RoadTilesBotCollection.Add(RoadTileBPSlopeBot);
	RoadTilesBotCollection.Add(RoadTileBPStraight);

	RoadTilesStriaghtCollection.Add(RoadTileBPSlopeBot);
	RoadTilesStriaghtCollection.Add(RoadTileBPStraight);
	RoadTilesStriaghtCollection.Add(RoadTileBPCornerLeft);
	RoadTilesStriaghtCollection.Add(RoadTileBPCornerRight);

	for (int i = 0; i < AmountOfRoadPiecesAhead; i++)
	{
		AddRoadTile();
	}
}

void URoadGenerator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void URoadGenerator::AddRoadTile()
{
	FRotator rotatorAdjustment = NextSpawnPointData.Rotator;
	TSubclassOf<ARoadTile> roadTileBPToSpawn = RoadTileBPStraight;

	if (LastRoadTileType == RoadTileType::SlopeBottom && CurrentHillStatus == HillStatus::None)
	{
		CurrentHillStatus = HillStatus::StartBot;
		roadTileBPToSpawn = RoadTileBPStraight;
	}
	else if (CurrentHillStatus == HillStatus::StartBot)
	{
		int randomIndex = randomIndex = rand() % RoadTilesTopCollection.Num();
		roadTileBPToSpawn = RoadTilesTopCollection[randomIndex];

		if (roadTileBPToSpawn.GetDefaultObject()->GetRoadTileType() == RoadTileType::SlopeTop)
		{
			CurrentHillStatus = HillStatus::StartTop;
		}
	}
	else if (CurrentHillStatus == HillStatus::StartTop)
	{
		int randomIndex = randomIndex = rand() % RoadTilesTopCollection.Num();
		roadTileBPToSpawn = RoadTilesTopCollection[randomIndex];

		rotatorAdjustment = FRotator(0, NextSpawnPointData.Rotator.Yaw, NextSpawnPointData.Rotator.Roll);

		if (roadTileBPToSpawn.GetDefaultObject()->GetRoadTileType() == RoadTileType::SlopeTop)
		{
			CurrentHillStatus = HillStatus::EndTop;

		}
	}
	else if (CurrentHillStatus == HillStatus::EndTop)
	{
		int randomIndex = randomIndex = rand() % RoadTilesBotCollection.Num();
		roadTileBPToSpawn = RoadTilesBotCollection[randomIndex];

		rotatorAdjustment = FRotator(-4, NextSpawnPointData.Rotator.Yaw, NextSpawnPointData.Rotator.Roll);

		if (roadTileBPToSpawn.GetDefaultObject()->GetRoadTileType() == RoadTileType::SlopeBottom)
		{
			CurrentHillStatus = HillStatus::EndBot;
		}
	}
	else if (CurrentHillStatus == HillStatus::EndBot)
	{
		CurrentHillStatus = HillStatus::None;
	}

	// done with hill?
	if (CurrentHillStatus == HillStatus::None)
	{
		int randomIndex = randomIndex = rand() % RoadTilesStriaghtCollection.Num();
		roadTileBPToSpawn = RoadTilesStriaghtCollection[randomIndex];
		roadTileBPToSpawn = RoadTilesStriaghtCollection[randomIndex];
		// make possibly bigger corners than 1 tile
		if ((roadTileBPToSpawn.GetDefaultObject()->GetRoadTileType() == RoadTileType::CornerLeft ||
			roadTileBPToSpawn.GetDefaultObject()->GetRoadTileType() == RoadTileType::CornerRight) &&
			CornersLeft > 0)
		{
			if(CornersLeft == 0)
				CornersLeft = rand() % 5;

			CornersLeft--;

		} 	
		else//we want a straight unril count is 0
		{
			if (LastRoadTileType != RoadTileType::Straight || StraightLeftsUntilCorner > 0)
				roadTileBPToSpawn = RoadTileBPStraight;
			// if count is finally 0 then reset it to original amount of srtiaghts 
			// we can now do corners again if lastRoadTileType was not striaght
			if (StraightLeftsUntilCorner == 0)
				StraightLeftsUntilCorner = AmountOfStraightsUntilCorner;
			// if the last one was striaght lower the straightsLeft
			if (LastRoadTileType == RoadTileType::Straight)
				StraightLeftsUntilCorner--;
		}
	}

	SpawnNextRoadTile(rotatorAdjustment, roadTileBPToSpawn);
}

void URoadGenerator::SpawnNextRoadTile(FRotator& rotatorAdjustment, TSubclassOf<ARoadTile>& roadTileBPToSpawn)
{
	ARoadTile* nextRoadTile = GetWorld()->SpawnActor<ARoadTile>(roadTileBPToSpawn, NextSpawnPointData.Location, rotatorAdjustment);
	nextRoadTile->Init(this);

	if (CurrentRoadTile)
		CurrentRoadTile->NextTile = nextRoadTile;

	nextRoadTile->PerviousTile = CurrentRoadTile;

	LastRoadTileType = nextRoadTile->GetRoadTileType();
	NextSpawnPointData = nextRoadTile->GetAttachPointData();

	CurrentRoadTile = nextRoadTile;

	if ((SpawnOneCarDebug && !SpawnOneCarDebugCompleted))
	{
		SpawnOneCarDebugCompleted = true;
		SpawnAI();
	}
	else
	{
		int chance = rand() % 2;  // CHANCE TO SPAWN CAR ON A TILE
		if(chance == 0)
			SpawnAI();
	}
}

void URoadGenerator::SpawnAI()
{
	int forwardOrOncomming = rand() & 1; // 50 % forward 0, oncomming 1

	UArrowComponent* arrowDirection = forwardOrOncomming == 0 ? CurrentRoadTile->GetForwardSpawnPoint() : CurrentRoadTile->GetOncommingSpawnPoint();
	AAIWheeledVehiclePawn* aiVehicle = GetWorld()->SpawnActor<AAIWheeledVehiclePawn>(AICarBP, arrowDirection->GetComponentLocation(), arrowDirection->GetComponentRotation());
	
	if (aiVehicle == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawning AI car failed"));
		return;
	}

	aiVehicle->CurrentLane = forwardOrOncomming == 0 ? LaneStatus::ForwardRight : LaneStatus::OncomingRight;
	aiVehicle->SetCurrentRoad(CurrentRoadTile); // FIX THIS
	
	if (SpawnOneCarDebug)
		GetWorld()->GetFirstPlayerController()->Possess(aiVehicle);
}

//Only for debugging AI - only called when IsDebugTrack = true
void URoadGenerator::GenerateDebugTrack()
{
	ARoadTile* firstRoadTile = nullptr;
	for (int i = 0; i < 10; i++)
	{
		SpawnNextRoadTile(NextSpawnPointData.Rotator, RoadTileBPStraight);
		if (!firstRoadTile)
			firstRoadTile = CurrentRoadTile;
	}	
	for (int i = 0; i < 12; i++)
	{
		SpawnNextRoadTile(NextSpawnPointData.Rotator, RoadTileBPCornerRight);
	}	
	for (int i = 0; i < 2; i++)
	{
		SpawnNextRoadTile(NextSpawnPointData.Rotator, RoadTileBPSlopeBot);
	}
	SpawnNextRoadTile(NextSpawnPointData.Rotator, RoadTileBPStraight);
	for (int i = 0; i < 2; i++)
	{
		SpawnNextRoadTile(NextSpawnPointData.Rotator, RoadTileBPSlopeTop);
	}

	double rotatorAdjustmentYaw = 0.f;
	for (int i = 0; i < 2; i++)
	{
		FRotator rotatorAdjustment = FRotator(rotatorAdjustmentYaw, NextSpawnPointData.Rotator.Yaw, NextSpawnPointData.Rotator.Roll);
		SpawnNextRoadTile(rotatorAdjustment, RoadTileBPSlopeTop);
		rotatorAdjustmentYaw -= 4;
	}
	SpawnNextRoadTile(NextSpawnPointData.Rotator, RoadTileBPStraight);
	for (int i = 0; i < 2; i++)
	{
		SpawnNextRoadTile(NextSpawnPointData.Rotator, RoadTileBPSlopeBot);
	}
	for (int i = 0; i < 2; i++)
	{
		SpawnNextRoadTile(NextSpawnPointData.Rotator, RoadTileBPCornerLeft);
	}
	for (int i = 0; i < 14; i++)
	{
		SpawnNextRoadTile(NextSpawnPointData.Rotator, RoadTileBPCornerRight);
	}
	for (int i = 0; i < 2; i++)
	{
		SpawnNextRoadTile(NextSpawnPointData.Rotator, RoadTileBPCornerLeft);
	}	
	for (int i = 0; i < 2; i++)
	{
		SpawnNextRoadTile(NextSpawnPointData.Rotator, RoadTileBPCornerRight);
	}

	//make it a loop
	CurrentRoadTile->NextTile = firstRoadTile;
	firstRoadTile->PerviousTile = CurrentRoadTile;
}


