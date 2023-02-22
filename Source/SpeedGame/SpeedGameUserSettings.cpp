// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeedGameUserSettings.h"


USpeedGameUserSettings::USpeedGameUserSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// default volume settings
	MasterVolume = 1.0f;
	MusicVolume =1.f;
	SFXVolume = 1.f;

	// default color settings
	RoadColor = FLinearColor::Red;
	SkyColor = FLinearColor::White;

	// default camera settings
	CameraSensitivity = 1;
	InvertedCameraSettings = FInvertedCameraSettings(false, false);
}


USpeedGameUserSettings* USpeedGameUserSettings::GetSpeedGameUserSettings()
{
	return Cast<USpeedGameUserSettings>(UGameUserSettings::GetGameUserSettings());
}

void USpeedGameUserSettings::SetAudioCategoryVolume(VolumeType volumeCategory, float newVolume)
{
	switch (volumeCategory)
	{
		case VolumeType::Master:
			MasterVolume = newVolume;
			break;
		case VolumeType::Music:
			MusicVolume = newVolume;
			break;
		case VolumeType::SFXEffect:
			SFXVolume = newVolume;
			break;
	}
}

void USpeedGameUserSettings::SetColorForSurfaceType(SurfaceType surfaceType, FLinearColor newColor)
{
	switch (surfaceType)
	{
		case SurfaceType::Road:
			RoadColor = newColor;
			break;
		case SurfaceType::Sky:
			SkyColor = newColor;
			break;
	}
}

void USpeedGameUserSettings::SetCameraSensitivitySettings(float sensitivity)
{
	CameraSensitivity = sensitivity;
}

void USpeedGameUserSettings::SetCameraInvetedSettings(FInvertedCameraSettings invertedSettings)
{
	InvertedCameraSettings = invertedSettings;
}
