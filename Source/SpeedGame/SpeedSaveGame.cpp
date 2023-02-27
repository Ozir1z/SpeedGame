// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeedSaveGame.h"

USpeedSaveGame::USpeedSaveGame()
{
	if (HighScores.IsEmpty())
	{
		HighScores.Add(FHighScoreData(TEXT("keanu_woofs"), 10800.f));
		HighScores.Add(FHighScoreData(TEXT("turtlesarecool"), 9000.f));
		HighScores.Add(FHighScoreData(TEXT("GAYMERGOD666"), 7200.f));
		HighScores.Add(FHighScoreData(TEXT("tryHARD"), 3600.f));
		HighScores.Add(FHighScoreData(TEXT("Bilbeau"), 1800.f));

		HighScores.Add(FHighScoreData(TEXT("frogsarealsocool"), 600.f));
		HighScores.Add(FHighScoreData(TEXT("BALD_MAN"), 300.f));
		HighScores.Add(FHighScoreData(TEXT("xxUltImAtEGaMeRxx"), 120.f));
		HighScores.Add(FHighScoreData(TEXT("TTV_SweatyG4mer"), 60.f));
		HighScores.Add(FHighScoreData(TEXT("thisgamesux"), 30.f));
	}
}

void USpeedSaveGame::AddHighScore(int index, FHighScoreData highScoreData)
{
	TArray<FHighScoreData> highScoreTemp = HighScores;

	for (int i = 0; i < HighScores.Num(); i++)
	{
		if (index <= i && i < HighScores.Num()-1)
		{
			HighScores[i] = highScoreTemp[i + 1]; // move items on or higher on the list 1 up
		}
	}
	HighScores.Insert(highScoreData, index);
	
	if (HighScores.Num() > 10)
		HighScores.Pop();
}

TArray<FHighScoreData> USpeedSaveGame::GetHighScores()
{
	return HighScores;
}

