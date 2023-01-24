// Copyright Epic Games, Inc. All Rights Reserved.


#include "SpeedGameGameModeBase.h"
#include "SpeedSaveGame.h"
#include "SpeedSaveGame.h"

ASpeedGameGameModeBase::ASpeedGameGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
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
	IsTimerGoing = false;// make enum?
	//getHighscore and show highscore UI and check if player has highscore
	USpeedSaveGame* speedSaveGame = SpeedSaveGame.GetDefaultObject();
	speedSaveGame->AddHighScore(9, FHighScoreData(TEXT("Test"), Timer));
	speedSaveGame->GetHighScores();
}

void ASpeedGameGameModeBase::StartGame()
{
	IsTimerGoing = true;
}
