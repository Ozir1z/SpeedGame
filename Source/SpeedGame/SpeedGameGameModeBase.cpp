// Copyright Epic Games, Inc. All Rights Reserved.


#include "SpeedGameGameModeBase.h"

void ASpeedGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	ChangeMenuWidget(StartingWidgetClass);
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

void ASpeedGameGameModeBase::ShowCurrentSpeed(int64 speed)
{
	ASpeedGameGameModeBase::ShowMPH(speed);
}
