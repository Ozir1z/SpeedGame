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
	void ChangeMenuWidget(UUserWidget* NewWidgetClass);

	void UpdateSpeed(int speed);
	void UpdateTimer(float deltaSeconds);

	void StopGame();
	void StartGame();

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Road Generation | Road")
	FLinearColor TrackColor;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speedgame | UMG")
	UUserWidget* CurrentWidget;

	UFUNCTION(BlueprintImplementableEvent, Category = "Speedgame | UMG")
	void UpdateSpeedOnUI(int speed);

	UFUNCTION(BlueprintImplementableEvent, Category = "Speedgame | UMG")
	void UpdateTimnerOnUI(float speed);

	UFUNCTION(BlueprintImplementableEvent, Category = "Speedgame | UMG")
	void ShowHighScoresOnUI(const TArray<FHighScoreData> &highscores, int indexToSetName);

	UFUNCTION(BlueprintCallable, Category = "Speedgame | SaveGame")
	void AddHighScore(FString PlayerNameToSave);

private:
	bool IsTimerGoing = false;
	float Timer = 0;

	int PlayerIndexToSetName;
};
