// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeedGameInstance.h"
#include <Kismet/GameplayStatics.h>

USpeedGameInstance::USpeedGameInstance()
{
    SaveGameSlotName = "SPEED_SAVE_012";

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
        SaveGameObject->Init();

        // Call SaveGameToSlot to serialize and save our SaveGameObject with name: <SaveGameSlotName>.sav
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


//void USpeedGameInstance::Init()
//{
//        SaveGameObject->HighScores.Init(FHighScoreData("test", 0), 10);
//    //if (SaveGameObject->HighScores.IsEmpty())
//    //{
//        SaveGameObject->HighScores.Add(FHighScoreData(TEXT("xxUltImAtEbUssErxx"), 86400.f));
//        SaveGameObject->HighScores.Add(FHighScoreData(TEXT("YoMaMa"), 36000.f));
//        SaveGameObject->HighScores.Add(FHighScoreData(TEXT("HellBOI"), 18000.f));
//        SaveGameObject->HighScores.Add(FHighScoreData(TEXT("SusAF"), 3600.f));
//        SaveGameObject->HighScores.Add(FHighScoreData(TEXT("Bilbeau"), 1800.f));
//
//        SaveGameObject->HighScores.Add(FHighScoreData(TEXT("SLAYERr"), 900.f));
//        SaveGameObject->HighScores.Add(FHighScoreData(TEXT("Bald"), 600.f));
//        SaveGameObject->HighScores.Add(FHighScoreData(TEXT("FranK"), 300.f));
//        SaveGameObject->HighScores.Add(FHighScoreData(TEXT("TTV_SweatyG4mer"), 60.f));
//        SaveGameObject->HighScores.Add(FHighScoreData(TEXT("thisG4meSux"), 30.f));
//    //}
//}