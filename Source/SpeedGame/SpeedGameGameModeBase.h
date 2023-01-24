// Coyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "SpeedGameGameModeBase.generated.h"




UCLASS()
class SPEEDGAME_API ASpeedGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	ASpeedGameGameModeBase();
public:
	UFUNCTION(BlueprintCallable, Category = "SpeedGame | UMG")
	void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);

	void UpdateSpeed(int speed);
	void UpdateTimer(float deltaSeconds);

	void StopGame();
	void StartGame();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpeedGame | SaveGame" )
	TSubclassOf<class USpeedSaveGame> SpeedSaveGame;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpeedGame | UMG")
	TSubclassOf<UUserWidget> StartingWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpeedGame | UMG")
	UUserWidget* CurrentWidget;

	UFUNCTION(BlueprintImplementableEvent, Category = "Speedgame | UMG")
	void UpdateSpeedOnUI(int speed);

	UFUNCTION(BlueprintImplementableEvent, Category = "Speedgame | UMG")
	void UpdateTimnerOnUI(float speed);

private:
	bool IsTimerGoing = true; // make this false and call it when driving off starting platform
	float Timer = 0;
};
