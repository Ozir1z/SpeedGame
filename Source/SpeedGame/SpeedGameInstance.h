// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SpeedSaveGame.h"
#include "SpeedGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SPEEDGAME_API USpeedGameInstance : public UGameInstance
{
	GENERATED_BODY()

	USpeedGameInstance();

public:
	UPROPERTY(BlueprintReadOnly)
	FString SaveGameSlotName;;

	UPROPERTY(BlueprintReadWrite)
	USpeedSaveGame* SaveGameObject;

	UFUNCTION(BlueprintCallable, Category = "Game Manager")
	void LoadGame();

	UFUNCTION(BlueprintCallable, Category = "Game Manager")
	void SaveGame();

private:
	void LogIfGameWasSavedOrNot(bool isSaved);

	//void Init();
};
