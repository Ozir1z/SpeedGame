// Fill out your copyright notice in the Description page of Project Settings.

#include "RoadGenerator.h"
#include "RoadTile.h"
#include "AIWheeledVehiclePawn.h"
#include "Components/ArrowComponent.h"
#include "SpeedGameGameModeBase.h"
#include "SpeedGameUserSettings.h"

URoadGenerator::URoadGenerator()
{
	PrimaryComponentTick.bCanEverTick = true;

	FirstSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("FirstSpawnPoint"));
	TrialTrackStartPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("TrialTrackStartPoint"));
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
	
	FirstSpawnPoint->SetRelativeLocation(FVector(3000, 0, 0));
	TrialTrackStartPoint->SetRelativeLocationAndRotation(FVector(0, 3000, 0), FRotator(0,90,0));

	RoadTilesTopCollection.Add(RoadTileBPSlopeTop);
	RoadTilesTopCollection.Add(RoadTileBPStraight);

	RoadTilesBotCollection.Add(RoadTileBPSlopeBot);
	RoadTilesBotCollection.Add(RoadTileBPStraight);

	RoadTilesStraightCollection.Add(RoadTileBPSlopeBot);
	RoadTilesStraightCollection.Add(RoadTileBPStraight);
	RoadTilesStraightCollection.Add(RoadTileBPCornerLeft);
	RoadTilesStraightCollection.Add(RoadTileBPCornerRight);
}

void URoadGenerator::Init()
{
	NextSpawnPointData = FAttachPointData{
		FirstSpawnPoint->GetComponentLocation(),
		FirstSpawnPoint->GetComponentQuat().Rotator()
	};
	DeleteTrialTrack();
	for (int i = 0; i < (AmountOfRoadPiecesAhead + InitialStraight); i++)
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

	
	if (InitialStraight > 0)
	{
		SpawnNextRoadTile(rotatorAdjustment, roadTileBPToSpawn);
		InitialStraight--;
		return;
	}

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
			CurrentHillStatus = HillStatus::StartTop;
	}
	else if (CurrentHillStatus == HillStatus::StartTop)
	{
		int randomIndex = randomIndex = rand() % RoadTilesTopCollection.Num();
		roadTileBPToSpawn = RoadTilesTopCollection[randomIndex];

		rotatorAdjustment = FRotator(0, NextSpawnPointData.Rotator.Yaw, NextSpawnPointData.Rotator.Roll);

		if (roadTileBPToSpawn.GetDefaultObject()->GetRoadTileType() == RoadTileType::SlopeTop)
			CurrentHillStatus = HillStatus::EndTop;
	}
	else if (CurrentHillStatus == HillStatus::EndTop)
	{
		int randomIndex = randomIndex = rand() % RoadTilesBotCollection.Num();
		roadTileBPToSpawn = RoadTilesBotCollection[randomIndex];

		rotatorAdjustment = FRotator(-4, NextSpawnPointData.Rotator.Yaw, NextSpawnPointData.Rotator.Roll);

		if (roadTileBPToSpawn.GetDefaultObject()->GetRoadTileType() == RoadTileType::SlopeBottom)
			CurrentHillStatus = HillStatus::EndBot;
	}
	else if (CurrentHillStatus == HillStatus::EndBot)
		CurrentHillStatus = HillStatus::None;

	if (CurrentHillStatus == HillStatus::None)
	{
		int randomIndex = randomIndex = rand() % RoadTilesStraightCollection.Num();

		// make possibly bigger corners than 1 tile
		if ((LastRoadTileType == RoadTileType::CornerLeft ||
			LastRoadTileType == RoadTileType::CornerRight) &&
			CornersLeft > 0)
		{
			roadTileBPToSpawn = LastRoadTileType == RoadTileType::CornerLeft ? RoadTileBPCornerLeft : RoadTileBPCornerRight;
			CornersLeft--;
		} 	
		else if (LastRoadTileType == RoadTileType::Straight
			&& StraightLeftsUntilOther > 0)
		{
			roadTileBPToSpawn = RoadTileBPStraight;
			StraightLeftsUntilOther--;
		}

		if (StraightLeftsUntilOther == 0)
		{
			StraightLeftsUntilOther = AmountOfStraightsUntilOther;
			roadTileBPToSpawn = RoadTilesStraightCollection[randomIndex];
		}

		if (CornersLeft == 0)
		{
			roadTileBPToSpawn = RoadTileBPStraight;
			CornersLeft = rand() % 5;
		}
	}

	SpawnNextRoadTile(rotatorAdjustment, roadTileBPToSpawn);
}

void URoadGenerator::SpawnNextRoadTile(FRotator& rotatorAdjustment, TSubclassOf<ARoadTile>& roadTileBPToSpawn, bool isTrialTrack)
{
	ARoadTile* nextRoadTile = GetWorld()->SpawnActor<ARoadTile>(roadTileBPToSpawn, NextSpawnPointData.Location, rotatorAdjustment);

	USpeedGameUserSettings* settings = USpeedGameUserSettings::GetSpeedGameUserSettings();
	if (!settings)
		return;

	nextRoadTile->Init(this, settings->GetRoadColor());
	nextRoadTile->IsTrialtrack = isTrialTrack;
	LastRoadTileType = nextRoadTile->GetRoadTileType();
	NextSpawnPointData = nextRoadTile->GetAttachPointData();

	if (isTrialTrack)
	{
		if (TrialTrackRoadtile)
			TrialTrackRoadtile->NextTile = nextRoadTile;

		nextRoadTile->PreviousTile = TrialTrackRoadtile;
		TrialTrackRoadtile = nextRoadTile;
	}	
	else
	{
		if (CurrentRoadTile)
			CurrentRoadTile->NextTile = nextRoadTile;

		nextRoadTile->PreviousTile = CurrentRoadTile;
		CurrentRoadTile = nextRoadTile;
	}
		

	int chance = rand() % 2;  // CHANCE TO SPAWN CAR ON A TILE
	if (chance == 0 && (InitialStraight <= 0 || isTrialTrack))
		nextRoadTile->SpawnCar(AICarBP);
}


void URoadGenerator::GenerateTrialTrack()
{
	ARoadTile* firstTrialTrackTile = nullptr;

	NextSpawnPointData = FAttachPointData{
		TrialTrackStartPoint->GetComponentLocation(),
		TrialTrackStartPoint->GetComponentQuat().Rotator()
	};
	for (int i = 0; i < 7; i++)
	{
		SpawnNextRoadTile(NextSpawnPointData.Rotator, RoadTileBPStraight, true);		
		if(!firstTrialTrackTile)
			firstTrialTrackTile = TrialTrackRoadtile;
	}	
	for (int i = 0; i < 12; i++)
	{
		SpawnNextRoadTile(NextSpawnPointData.Rotator, RoadTileBPCornerRight, true);
	}	
	for (int i = 0; i < 2; i++)
	{
		SpawnNextRoadTile(NextSpawnPointData.Rotator, RoadTileBPSlopeBot, true);
	}
	SpawnNextRoadTile(NextSpawnPointData.Rotator, RoadTileBPStraight, true);
	for (int i = 0; i < 2; i++)
	{
		SpawnNextRoadTile(NextSpawnPointData.Rotator, RoadTileBPSlopeTop, true);
	}

	double rotatorAdjustmentYaw = 0.f;
	for (int i = 0; i < 2; i++)
	{
		FRotator rotatorAdjustment = FRotator(rotatorAdjustmentYaw, NextSpawnPointData.Rotator.Yaw, NextSpawnPointData.Rotator.Roll);
		SpawnNextRoadTile(rotatorAdjustment, RoadTileBPSlopeTop, true);
		rotatorAdjustmentYaw -= 4;
	}
	SpawnNextRoadTile(NextSpawnPointData.Rotator, RoadTileBPStraight, true);
	for (int i = 0; i < 2; i++)
	{
		SpawnNextRoadTile(NextSpawnPointData.Rotator, RoadTileBPSlopeBot, true);
	}
	for (int i = 0; i < 2; i++)
	{
		SpawnNextRoadTile(NextSpawnPointData.Rotator, RoadTileBPCornerLeft, true);
	}
	for (int i = 0; i < 14; i++)
	{
		SpawnNextRoadTile(NextSpawnPointData.Rotator, RoadTileBPCornerRight, true);
	}
	for (int i = 0; i < 2; i++)
	{
		SpawnNextRoadTile(NextSpawnPointData.Rotator, RoadTileBPCornerLeft, true);
	}	
	for (int i = 0; i < 2; i++)
	{
		SpawnNextRoadTile(NextSpawnPointData.Rotator, RoadTileBPCornerRight, true);
	}

	TrialTrackRoadtile->NextTile = firstTrialTrackTile;
	firstTrialTrackTile->PreviousTile = TrialTrackRoadtile;
}


void URoadGenerator::DeleteTrialTrack()
{
	if (!TrialTrackRoadtile)
		return;

	ARoadTile* firstTile = TrialTrackRoadtile;
	bool doneFirst = false;

	while (TrialTrackRoadtile != firstTile || !doneFirst)
	{
		doneFirst = true;
		TrialTrackRoadtile->Destroy();
		TrialTrackRoadtile = TrialTrackRoadtile->NextTile;
		TrialTrackRoadtile->PreviousTile = nullptr;
	}
	TrialTrackRoadtile = nullptr;
}

void URoadGenerator::UpdateTrialTrack(FLinearColor newColor)
{
	if (!TrialTrackRoadtile)
		return;

	ARoadTile* firstTile = TrialTrackRoadtile;
	bool doneFirst = false;

	while (TrialTrackRoadtile != firstTile || !doneFirst)
	{
		doneFirst = true;
		TrialTrackRoadtile->Init(this, newColor);
		TrialTrackRoadtile = TrialTrackRoadtile->NextTile;
	}
	TrialTrackRoadtile = firstTile;
}
