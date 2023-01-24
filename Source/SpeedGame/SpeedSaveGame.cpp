// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeedSaveGame.h"

USpeedSaveGame::USpeedSaveGame()
{
//	HighScores.Init(FHighScoreData("Test", 1999),0);
	HighScores.SetNum(10);
	HighScores.Add(FHighScoreData(TEXT("Bob"), 86400));
	HighScores.Add(FHighScoreData(TEXT("Bob"), 36000));
	HighScores.Add(FHighScoreData(TEXT("Bob"), 18000));
	HighScores.Add(FHighScoreData(TEXT("Bob"), 3600));
	HighScores.Add(FHighScoreData(TEXT("Bob"), 1800));

	HighScores.Add(FHighScoreData(TEXT("Bob"), 900));
	HighScores.Add(FHighScoreData(TEXT("Bob"), 600));
	HighScores.Add(FHighScoreData(TEXT("Bob"), 300));
	HighScores.Add(FHighScoreData(TEXT("Bob"), 60));
	HighScores.Add(FHighScoreData(TEXT("Bob"), 30));
}

void USpeedSaveGame::AddHighScore(int index, FHighScoreData highScoreData)
{
	HighScores.Insert(highScoreData, index);
}

TArray<FHighScoreData> USpeedSaveGame::GetHighScores()
{
	return HighScores;
}

