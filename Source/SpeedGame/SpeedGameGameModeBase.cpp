// Copyright Epic Games, Inc. All Rights Reserved.


#include "SpeedGameGameModeBase.h"
#include "SpeedSaveGame.h"
#include <Kismet/GameplayStatics.h>

ASpeedGameGameModeBase::ASpeedGameGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	if (UGameplayStatics::DoesSaveGameExist(SaveName, 0))
		SaveGame= UGameplayStatics::LoadGameFromSlot(SaveName, 0);
	else
		SaveGame = UGameplayStatics::CreateSaveGameObject(SpeedSaveGame);

}

void ASpeedGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	ChangeMenuWidget(StartingWidgetClass);
}

void ASpeedGameGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateTimer(DeltaTime);
}

void ASpeedGameGameModeBase::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromParent();
		CurrentWidget = nullptr;
	}
	if (NewWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

void ASpeedGameGameModeBase::UpdateSpeed(int speed)
{
	ASpeedGameGameModeBase::UpdateSpeedOnUI(speed);
}

void ASpeedGameGameModeBase::UpdateTimer(float deltaSeconds)
{
	if (!IsTimerGoing) return; //dont update timer if we havent started yet

	Timer += deltaSeconds;
	ASpeedGameGameModeBase::UpdateTimnerOnUI(Timer);
}

void ASpeedGameGameModeBase::StopGame()
{
	if (IsTimerGoing)
	{
		USpeedSaveGame* speedSaveGame = SpeedSaveGame.GetDefaultObject();
		TArray<FHighScoreData> highscores = speedSaveGame->GetHighScores();

		PlayerIndexToSetName = -1;
		for (int i= 0; i < highscores.Num(); i++)
		{
			if (Timer > highscores[i].Score && PlayerIndexToSetName == -1)
			{
				PlayerIndexToSetName = i;
				highscores.Insert(FHighScoreData(TEXT(""), Timer), PlayerIndexToSetName);
				highscores.Pop();
				break;
			}
		}

		ASpeedGameGameModeBase::ShowHighScoresOnUI(highscores, PlayerIndexToSetName);
	}

	IsTimerGoing = false;// make enum?
}

void ASpeedGameGameModeBase::StartGame()
{
	IsTimerGoing = true;
}


void ASpeedGameGameModeBase::AddHighScore(FString PlayerNameToSave)
{
	if (PlayerIndexToSetName == -1)
		return;

	USpeedSaveGame* speedSaveGame = SpeedSaveGame.GetDefaultObject();
	speedSaveGame->GetHighScores()[PlayerIndexToSetName].Name = PlayerNameToSave;

	UGameplayStatics::SaveGameToSlot(SaveGame, SaveName, 0);
}