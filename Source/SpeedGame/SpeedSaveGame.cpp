// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeedSaveGame.h"

USpeedSaveGame::USpeedSaveGame()
{
	HighScores.Empty();
	if (HighScores.IsEmpty())
	{
		HighScores.Add(FHighScoreData(TEXT("xxUltImAtEbUssErxx"), 86400.f));
		HighScores.Add(FHighScoreData(TEXT("YoMaMa"), 36000.f));
		HighScores.Add(FHighScoreData(TEXT("HellBOI"), 18000.f));
		HighScores.Add(FHighScoreData(TEXT("SusAF"), 3600.f));
		HighScores.Add(FHighScoreData(TEXT("Bilbeau"), 1800.f));

		HighScores.Add(FHighScoreData(TEXT("SLAYERr"), 900.f));
		HighScores.Add(FHighScoreData(TEXT("Bald"), 600.f));
		HighScores.Add(FHighScoreData(TEXT("FranK"), 300.f));
		HighScores.Add(FHighScoreData(TEXT("TTV_SweatyG4mer"), 60.f));
		HighScores.Add(FHighScoreData(TEXT("thisG4meSux"), 30.f));
	}
}

void USpeedSaveGame::AddHighScore(int index, FHighScoreData highScoreData)
{
	//HighScores.Insert(highScoreData, index);
	//HighScores.Pop();
}

TArray<FHighScoreData> USpeedSaveGame::GetHighScores()
{
	return HighScores;
}

