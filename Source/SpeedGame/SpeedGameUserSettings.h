// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "SpeedGameUserSettings.generated.h"


UENUM(BlueprintType)
enum class VolumeType : uint8
{
	Master		UMETA(DisplayName = "Master Volume"),
	Music		UMETA(DisplayName = "Music Volume"),
	SFXEffect	UMETA(DisplayName = "SFX Effect Volume")
};

UENUM(BlueprintType)
enum class SurfaceType : uint8
{
	Road	UMETA(DisplayName = "Road"),
	Sky		UMETA(DisplayName = "Sky")
};

USTRUCT(BlueprintType)
struct FInvertedCameraSettings
{
	GENERATED_USTRUCT_BODY()

public:
	FInvertedCameraSettings()
	{

	}

	FInvertedCameraSettings(float x, float y)
	{
		x = X;
		y = Y;
	}

	UPROPERTY()
	bool X;
	UPROPERTY()
	bool Y;
};


UCLASS(config = SpeedGameUserSettings, configdonotcheckdefaults)
class SPEEDGAME_API USpeedGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

	USpeedGameUserSettings(const FObjectInitializer& ObjectInitializer);

public:
	UFUNCTION(BlueprintPure)
	static USpeedGameUserSettings* GetSpeedGameUserSettings();

	// Volume settings
	UFUNCTION(BlueprintCallable, Category = SpeedGameSettings)
	void SetAudioCategoryVolume(VolumeType volumeCategory, float newVolume);

	UFUNCTION(BlueprintPure, Category = SpeedGameSettings)
	float GetMasterVolume() const { return MasterVolume; }

	UFUNCTION(BlueprintPure, Category = SpeedGameSettings)
	float GetMusicVolume() const { return MusicVolume; }

	UFUNCTION(BlueprintPure, Category = SpeedGameSettings)
	float GetSFXVolume() const { return SFXVolume; }


	// Color settings
	UFUNCTION(BlueprintCallable, Category = SpeedGameSettings)
	void SetColorForSurfaceType(SurfaceType surfaceType, FLinearColor newColor);

	UFUNCTION(BlueprintPure, Category = SpeedGameSettings)
	FLinearColor GetRoadColor() const { return RoadColor; }

	UFUNCTION(BlueprintPure, Category = SpeedGameSettings)
	FLinearColor GetSkyColor() const { return SkyColor; }


	// Camera settings 
	UFUNCTION(BlueprintCallable, Category = SpeedGameSettings)
	void SetCameraSensitivitySettings(float sensitivity);

	UFUNCTION(BlueprintCallable, Category = SpeedGameSettings)
	void SetCameraInvetedSettings(FInvertedCameraSettings invertedSettings);

	UFUNCTION(BlueprintPure, Category = SpeedGameSettings)
	float GetCameraSensitivity() const { return CameraSensitivity; }

	UFUNCTION(BlueprintPure, Category = SpeedGameSettings)
	FInvertedCameraSettings GetInvertedCameraX() const { return InvertedCameraSettings; }

protected:
	UPROPERTY(config)
	float MasterVolume;

	UPROPERTY(config)
	float MusicVolume;

	UPROPERTY(config)
	float SFXVolume;

	UPROPERTY(config)
	FLinearColor RoadColor;

	UPROPERTY(config)
	FLinearColor SkyColor;

	UPROPERTY(Config)
	float CameraSensitivity;

	UPROPERTY(Config)
	FInvertedCameraSettings InvertedCameraSettings;
};
