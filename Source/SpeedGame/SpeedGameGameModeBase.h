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

public:
	UFUNCTION(BlueprintCallable, Category = "SpeedGame UMG")
	void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);

	void ShowCurrentSpeed(int64 speed);



protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpeedGame UMG")
	TSubclassOf<UUserWidget> StartingWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpeedGame UMG")
	UUserWidget* CurrentWidget;

	UFUNCTION(BlueprintImplementableEvent, Category = "Speedgame MPH")
	void ShowMPH(int64 speed);

private:

};
