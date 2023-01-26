// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SpeedSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class SPEEDGAME_API USpeedSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	USpeedSaveGame();

	void AddHighScore(int index, FHighScoreData highScoreData);
	TArray<FHighScoreData> GetHighScores();

private:
	TArray<FHighScoreData> HighScores;
};


USTRUCT(BlueprintType)
struct FHighScoreData
{
	GENERATED_USTRUCT_BODY()

	FHighScoreData() {

	}

	FHighScoreData(FString name, float score) {
		Name = name;
		Score = score;
	}

	UPROPERTY(BlueprintReadOnly, Category = "Speed Game | HighScore")
	FString Name;
	UPROPERTY(BlueprintReadOnly, Category = "Speed Game | HighScore")
	float Score;
};