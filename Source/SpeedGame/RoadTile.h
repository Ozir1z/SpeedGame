// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	
	bool IsTrialtrack = false;
	void Init(class URoadGenerator* roadGenerator, FLinearColor color);
	void SpawnCar(TSubclassOf<class AAIWheeledVehiclePawn> aiCarBP);

	FAttachPointData GetAttachPointData();
	RoadTileType GetRoadTileType();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpeedGame | tiles")
	ARoadTile* NextTile = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpeedGame | tiles")
	ARoadTile* PreviousTile = nullptr;

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

	static const FName ForwardTriggerName;
	static const FName OncommingTriggerName;
	static const FName LeftSideTriggerName;
	static const FName RightSideTriggerName;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, Category = "SpeedGame | Material")
	UMaterialInterface* Material;

	UPROPERTY(EditAnywhere)
	class USceneComponent* CustomRootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
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

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* LeftSideTriggerBox;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* RightSideTriggerBox;

	UFUNCTION()
	void OnOverlapForwardBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category = "SpeedGame | RoadType")
	RoadTileType RoadTileType = RoadTileType::None;

private:
	class URoadGenerator* RoadGenerator;
	UMaterialInstanceDynamic* DynamicMaterial_RoadMesh;

	void GenerateAndDestroyRoad();

	float DestroyTimer = 20;
	bool TimetoDestroy = false;
};

USTRUCT(Atomic)
struct FAttachPointData
{
	GENERATED_USTRUCT_BODY()

public:
	FAttachPointData() 
	{

	}

	FAttachPointData(FVector location, FRotator rotator)
	{
		Location = location;
		Rotator = rotator;
	}
	
	UPROPERTY()
	FVector Location;
	UPROPERTY()
	FRotator Rotator;
};