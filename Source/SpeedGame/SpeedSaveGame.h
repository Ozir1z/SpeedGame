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

	//FHighScoreData* TryToGetHighScoreDataToReplace(FHighScoreData score);
	
	TArray<FHighScoreData> GetHighScores();

private:
	TArray<FHighScoreData> HighScores;
};


USTRUCT(Atomic)
struct FHighScoreData
{
	GENERATED_USTRUCT_BODY()

	FHighScoreData() {

	}

	FHighScoreData(FString name, int64 seconds) {
		Name = name;
		Seconds = seconds;
	}

	UPROPERTY()
	FString Name;
	UPROPERTY()
	int64 Seconds;
};