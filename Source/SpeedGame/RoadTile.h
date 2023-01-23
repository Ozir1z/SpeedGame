// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIWheeledVehiclePawn.h"
#include "RoadTile.generated.h"

UENUM(BlueprintType)
enum class RoadTileType : uint8
{
	None UMETA(DisplayName = "None"),
	Straight UMETA(DisplayName = "Straight"),
	CornerRight UMETA(DisplayName = "CornerRight"),
	CornerLeft UMETA(DisplayName = "CornerLeft"),
	SlopeBottom UMETA(DisplayName = "SlopeBottom"),
	SlopeTop UMETA(DisplayName = "SlopeTop")
};

UCLASS()
class SPEEDGAME_API ARoadTile : public AActor
{
	GENERATED_BODY()
	
public:	
	ARoadTile();
	virtual void Tick(float DeltaTime) override;
	
	void Init(class URoadGenerator* roadGenerator);

	FAttachPointData GetAttachPointData();
	RoadTileType GetRoadTileType();
	
	UPROPERTY()
	ARoadTile* NextTile = nullptr;
	UPROPERTY()
	ARoadTile* PerviousTile = nullptr;

	class UArrowComponent* GetForwardSpawnPoint();
	class UArrowComponent* GetOncommingSpawnPoint();

	UPROPERTY(VisibleAnywhere, Category = "SpeedGame | lanes")
	class USplineComponent* ForwardRightLane;

	UPROPERTY(VisibleAnywhere, Category = "SpeedGame | lanes")
	class USplineComponent* ForwardLefttLane;

	UPROPERTY(VisibleAnywhere, Category = "SpeedGame | lanes")
	class USplineComponent* OncommingLeftLane;

	UPROPERTY(VisibleAnywhere, Category = "SpeedGame | lanes")
	class USplineComponent* OncommingRightLane;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class USphereComponent* CustomRootComponent;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* RoadMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UArrowComponent* NextRoadPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UArrowComponent* SpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UArrowComponent* SpawnPointOncomming;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* ForwardTriggerBox;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* OncommingTriggerBox;

	UFUNCTION()
	void OnOverlapForwardBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapOncomingBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category = "SpeedGame | RoadType")
	RoadTileType RoadTileType = RoadTileType::None;

private:
	class URoadGenerator* RoadGenerator;

	void GenerateAndDestroyRoad();
	void SetCurrentRoadTileForVehicleOrDestroy(class ARoadTile* roadTileToSet, class AAIWheeledVehiclePawn* aiVehicle, LaneStatus LeftLaneStatus,LaneStatus RightLaneStatus);
};

USTRUCT(Atomic)
struct FAttachPointData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector Location;
	UPROPERTY()
	FRotator Rotator;
};