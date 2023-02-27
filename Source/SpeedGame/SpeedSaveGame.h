// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SpeedSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FHighScoreData
{
	GENERATED_BODY()

	FHighScoreData() {

	}

	FHighScoreData(FString name, float score) {
		Name = name;
		Score = score;
	}


public:
	UPROPERTY(BlueprintReadOnly, Category = "Speed Game | HighScore")
	FString Name;
	UPROPERTY(BlueprintReadOnly, Category = "Speed Game | HighScore")
	float Score;
};


UCLASS()
class SPEEDGAME_API USpeedSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	USpeedSaveGame();;

	void AddHighScore(int index, FHighScoreData highScoreData);
	TArray<FHighScoreData> GetHighScores();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed Game | Highscores")
	TArray<FHighScoreData> HighScores;
};
