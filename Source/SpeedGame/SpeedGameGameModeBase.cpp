// Copyright Epic Games, Inc. All Rights Reserved.


#include "SpeedGameGameModeBase.h"
#include "SpeedSaveGame.h"
#include "SpeedGameInstance.h"

ASpeedGameGameModeBase::ASpeedGameGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASpeedGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	USpeedGameInstance* gameInstance = Cast<USpeedGameInstance>(GetWorld()->GetGameInstance());
	gameInstance->LoadGame();

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
	if (!IsTimerGoing) return;

	IsTimerGoing = false;// make enum?
	USpeedGameInstance* gameInstance = Cast<USpeedGameInstance>(GetWorld()->GetGameInstance());
	TArray<FHighScoreData> highscores = gameInstance->SaveGameObject->GetHighScores();

	PlayerIndexToSetName = -1;
	for (int i= 0; i < highscores.Num(); i++)
	{
		if (Timer > highscores[i].Score && PlayerIndexToSetName == -1)
		{
			PlayerIndexToSetName = i;
			gameInstance->SaveGameObject->AddHighScore(PlayerIndexToSetName, FHighScoreData(TEXT(""), Timer));
			break;
		}
	}

	ASpeedGameGameModeBase::ShowHighScoresOnUI(gameInstance->SaveGameObject->HighScores, PlayerIndexToSetName);
}

void ASpeedGameGameModeBase::StartGame()
{
	IsTimerGoing = true;
}

void ASpeedGameGameModeBase::AddHighScore(FString PlayerNameToSave)
{
	if (PlayerIndexToSetName == -1)
		return;

	USpeedGameInstance* gameInstance = Cast<USpeedGameInstance>(GetWorld()->GetGameInstance());
	gameInstance->SaveGameObject->AddHighScore(PlayerIndexToSetName, FHighScoreData(PlayerNameToSave, Timer));
	gameInstance->SaveGame();
}
