// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RoadTile.h"
#include "RoadGenerator.generated.h"

UENUM()
enum class HillStatus : uint8
{
	None UMETA(DisplayName = "None"),
	StartBot UMETA(DisplayName = "StartBot"),
	StartTop UMETA(DisplayName = "StartTop"),
	EndTop UMETA(DisplayName = "EndTop"),
	EndBot UMETA(DisplayName = "EndBot")
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPEEDGAME_API URoadGenerator : public UActorComponent
{
	GENERATED_BODY()

		virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	URoadGenerator();
	void AddRoadTile();

	UPROPERTY(EditAnywhere, Category = "Road Generation | Car")
	bool SpawnOneCarDebug = false;

	UPROPERTY(EditAnywhere, Category = "Road Generation | Car")
	bool IsDebugTrack = false;


protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Road Generation | Road")
	class UArrowComponent* FirstSpawnPoint;

private:
	bool SpawnOneCarDebugCompleted = true;

	// ROAD
	UPROPERTY(EditAnywhere, Category = "Road Generation | Road")
	bool GenerateInitialRoad = true;

	UPROPERTY(EditDefaultsOnly, Category = "Road Generation | Road")
	TSubclassOf<ARoadTile> RoadTileBPStraight;

	UPROPERTY(EditDefaultsOnly, Category = "Road Generation | Road")
	TSubclassOf<ARoadTile> RoadTileBPCornerLeft;

	UPROPERTY(EditDefaultsOnly, Category = "Road Generation | Road")
	TSubclassOf<ARoadTile> RoadTileBPCornerRight;

	UPROPERTY(EditDefaultsOnly, Category = "Road Generation | Road")
	TSubclassOf<ARoadTile> RoadTileBPSlopeBot;

	UPROPERTY(EditDefaultsOnly, Category = "Road Generation | Road")
	TSubclassOf<ARoadTile> RoadTileBPSlopeTop;

	TArray<TSubclassOf<ARoadTile>> RoadTilesTopCollection;
	TArray<TSubclassOf<ARoadTile>> RoadTilesBotCollection;
	TArray<TSubclassOf<ARoadTile>> RoadTilesStriaghtCollection;

	UPROPERTY(EditDefaultsOnly, Category = "Road Generation | Road")
	int AmountOfRoadPiecesAhead = 20;

	HillStatus CurrentHillStatus = HillStatus::None;
	RoadTileType LastRoadTileType = RoadTileType::None;

	FAttachPointData NextSpawnPointData;
	ARoadTile* CurrentRoadTile = nullptr;
	void SpawnNextRoadTile(FRotator& rotatorAdjustment, TSubclassOf<ARoadTile>& roadTileBPToSpawn);

	int const AmountOfStraightsUntilCorner = 3;
	int StraightLeftsUntilCorner = AmountOfStraightsUntilCorner;
	int CornersLeft = CornersLeft = rand() % 4;

	//AI
	UPROPERTY(EditDefaultsOnly, Category = "Road Generation | Car")
	TSubclassOf<class AAIWheeledVehiclePawn> AICarBP;

	void SpawnAI();
	void GenerateDebugTrack();
};