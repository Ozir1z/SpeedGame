// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeedGameInstance.h"
#include <Kismet/GameplayStatics.h>

USpeedGameInstance::USpeedGameInstance()
{
    SaveGameSlotName = "SPEED_SAVE_GAME";
}

void USpeedGameInstance::LoadGame()
{
    USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(SaveGameSlotName, 0);
    SaveGameObject = Cast<USpeedSaveGame>(LoadedGame);

    UE_LOG(LogTemp, Warning, TEXT("Trying to load a saved game."));

    if (!SaveGameObject)
    {
        UE_LOG(LogTemp, Warning, TEXT("No saved games found. Trying to save a new one."));

        SaveGameObject = Cast<USpeedSaveGame>(UGameplayStatics::CreateSaveGameObject(USpeedSaveGame::StaticClass()));

        const bool IsSaved = UGameplayStatics::SaveGameToSlot(SaveGameObject, SaveGameSlotName, 0);

        LogIfGameWasSavedOrNot(IsSaved);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Saved game found. Loaded."));
    }
}

void USpeedGameInstance::SaveGame()
{
    UE_LOG(LogTemp, Warning, TEXT("Saving game..."));
    const bool IsSaved = UGameplayStatics::SaveGameToSlot(SaveGameObject, SaveGameSlotName, 0);

    LogIfGameWasSavedOrNot(IsSaved);
}

void USpeedGameInstance::LogIfGameWasSavedOrNot(bool isSaved)
{
    if(isSaved)
    {
        UE_LOG(LogTemp, Warning, TEXT("Game was saved succesfully"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Game was NOT saved succesfully"));
    }   
}