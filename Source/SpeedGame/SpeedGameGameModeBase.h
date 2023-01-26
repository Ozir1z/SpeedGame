// Coyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "SpeedSaveGame.h"
#include "SpeedGameGameModeBase.generated.h"


UCLASS()
class SPEEDGAME_API ASpeedGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	ASpeedGameGameModeBase();
public:
	UFUNCTION(BlueprintCallable, Category = "Speedgame | UMG")
	void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);

	void UpdateSpeed(int speed);
	void UpdateTimer(float deltaSeconds);

	void StopGame();
	void StartGame();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speedgame | UMG")
	TSubclassOf<UUserWidget> StartingWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speedgame | UMG")
	UUserWidget* CurrentWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Speedgame | SaveGame")
	TSubclassOf<class USpeedSaveGame> SpeedSaveGame;

	UFUNCTION(BlueprintImplementableEvent, Category = "Speedgame | UMG")
	void UpdateSpeedOnUI(int speed);

	UFUNCTION(BlueprintImplementableEvent, Category = "Speedgame | UMG")
	void UpdateTimnerOnUI(float speed);

	UFUNCTION(BlueprintImplementableEvent, Category = "Speedgame | UMG")
	void ShowHighScoresOnUI(const TArray<FHighScoreData> &highscores, int indexToSetName);

	UFUNCTION(BlueprintCallable, Category = "Speedgame | SaveGame")
	void AddHighScore(FString PlayerNameToSave);

	USaveGame* SaveGame;
private:
	bool IsTimerGoing = true; // make this false and call it when driving off starting platform
	float Timer = 0;

	const FString SaveName = "SpeedSaveGameFile";

	int PlayerIndexToSetName;
};
