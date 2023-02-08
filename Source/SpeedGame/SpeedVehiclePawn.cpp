// Fill out your copyright notice in the Description page of Project Settings.)

#include "SpeedVehiclePawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include <EnhancedInputSubsystems.h>
#include <ChaosVehicleMovementComponent.h>
#include <EnhancedInputComponent.h>
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "SpeedGameGameModeBase.h"
#include <Kismet/KismetStringLibrary.h>

ASpeedVehiclePawn::ASpeedVehiclePawn()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	CameraArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArmComponent"));
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	
	CameraArmComp->SetupAttachment(RootComponent);
	CameraComp->SetupAttachment(CameraArmComp, USpringArmComponent::SocketName);

	CameraArmComp->TargetArmLength = 2000.f;
	CameraArmComp->bUsePawnControlRotation = false;

}


void ASpeedVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (SteerAction)
		{
			PlayerEnhancedInputComponent->BindAction(SteerAction, ETriggerEvent::Triggered, this, &ASpeedVehiclePawn::Steer);
			PlayerEnhancedInputComponent->BindAction(SteerAction, ETriggerEvent::Completed, this, &ASpeedVehiclePawn::Steer);
		}
		if (ThrottleAction)
		{
			PlayerEnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ASpeedVehiclePawn::Throttle);
			PlayerEnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ASpeedVehiclePawn::Throttle);
		}
		if (BrakeAction)
		{
			PlayerEnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &ASpeedVehiclePawn::Brake);
			PlayerEnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ASpeedVehiclePawn::Brake);
		}	
		if (HandBrakeAction)
		{
			PlayerEnhancedInputComponent->BindAction(HandBrakeAction, ETriggerEvent::Started, this, &ASpeedVehiclePawn::HandBrake);
			PlayerEnhancedInputComponent->BindAction(HandBrakeAction, ETriggerEvent::Completed, this, &ASpeedVehiclePawn::HandBrake);
		}
		if (LookAction)
		{
			PlayerEnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASpeedVehiclePawn::Look);
		}
		if (CameraAction)
		{
			PlayerEnhancedInputComponent->BindAction(CameraAction, ETriggerEvent::Started, this, &ASpeedVehiclePawn::CameraDistance);
		}
	}	
}

void ASpeedVehiclePawn::BeginPlay()
{
	Super::BeginPlay();
	PlayerStartLocation = GetActorLocation();
	PlayerStartRotation = GetActorRotation();
}

void ASpeedVehiclePawn::Init()
{
	CurrentBombStatus = BombStatus::Inactive;
	FVector speedboost = GetActorForwardVector() * 2000;
	GetMesh()->AddImpulse(speedboost, NAME_None, true);
}

void ASpeedVehiclePawn::HandleVehicleGoingOffroad(bool isOnTrialTrack)
{
	if (isOnTrialTrack)
		return;

	if (CurrentBombStatus == BombStatus::Explodeded)
		return;

	Death();
	CurrentBombStatus = BombStatus::Explodeded;
}


void ASpeedVehiclePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentBombStatus == BombStatus::None && GetActorLocation().Z <= -500)
	{
		USkeletalMesh* originalMesh = GetMesh()->GetSkeletalMeshAsset();
		GetMesh()->SetSkeletalMeshAsset(SecondaryMesh);
		GetMesh()->SetSkeletalMeshAsset(originalMesh);
		GetMesh()->SetWorldLocationAndRotation(PlayerStartLocation, PlayerStartRotation, false, nullptr, ETeleportType::TeleportPhysics);
		return;
	}

	HandleVehicleSpeed();
	
	TimePassedSinceCameraInput += DeltaTime;
	if (TimePassedSinceCameraInput > 1.5f && CurrentBombStatus != BombStatus::Explodeded && CurrentCameraStatus == CameraStatus::Manual)
		SwitchCameraStatusTo(CameraStatus::Follow);
	if(CurrentBombStatus == BombStatus::Explodeded)
		SwitchCameraStatusTo(CameraStatus::Manual);
}

void ASpeedVehiclePawn::HandleVehicleSpeed()
{
	ASpeedGameGameModeBase* gameMode = (ASpeedGameGameModeBase*)GetWorld()->GetAuthGameMode();

	int speed = static_cast<int>(GetVehicleMovementComponent()->GetForwardSpeedMPH());
	if (speed <= 0) // make a maxspeed variable in gamemode, so we can make difficulties if we want to
		speed = 0;

	gameMode->UpdateSpeed(speed);
	if (CurrentBombStatus == BombStatus::None)
		return;

	if (speed >= GameSpeed && CurrentBombStatus == BombStatus::Inactive)
	{
		CurrentBombStatus = BombStatus::Active;
		gameMode->StartGame();
	}
	if (speed < GameSpeed && CurrentBombStatus == BombStatus::Active)
	{
		HandleVehicleGoingOffroad();
	}
	if (CurrentBombStatus == BombStatus::Explodeded)
	{
		speed = 0;
		gameMode->StopGame();
	}
}

void ASpeedVehiclePawn::Death()
{
	if (DeathParticle)
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DeathParticle, GetActorLocation(), GetActorRotation());
	double forwardSpeedAbs = UKismetMathLibrary::Abs(GetVehicleMovementComponent()->GetForwardSpeed());

	GetMesh()->SetEnableGravity(false);
	GetMesh()->SetSkeletalMeshAsset(SecondaryMesh);

	FVector impulse = (GetActorForwardVector() * forwardSpeedAbs) + (GetActorUpVector() * 500);
	GetMesh()->AddVelocityChangeImpulseAtLocation(impulse, GetActorLocation(), TEXT("Body"));
}

void ASpeedVehiclePawn::SwitchCameraStatusTo(CameraStatus newCameraStatus)
{
	CurrentCameraStatus = newCameraStatus;
	switch (CurrentCameraStatus) 
	{
		case CameraStatus::Manual:
		case CameraStatus::InMenu:
			CameraArmComp->bUsePawnControlRotation = true;
			break;
		case CameraStatus::Follow:
			CameraArmComp->SetRelativeRotation(FRotator(-15, 0, 0));
			GetController()->ClientSetRotation(CameraArmComp->GetComponentRotation());
			CameraArmComp->bEnableCameraRotationLag = true;
			CameraArmComp->bUsePawnControlRotation = false;
			break;
		case CameraStatus::FirstPerson:
			CameraArmComp->bEnableCameraRotationLag = false;
			CameraArmComp->SetRelativeRotation(FRotator(0, 0, 0));
			CameraArmComp->bUsePawnControlRotation = false;
			break;
	}
}


void ASpeedVehiclePawn::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			InputSystem->ClearAllMappings();
			InputSystem->AddMappingContext(InputMappingContext, InputMappingPriority);
		}
	}
}

void ASpeedVehiclePawn::Steer(const FInputActionInstance& ActionInstance)
{
	float ActionVector = ActionInstance.GetValue().Get<float>();
	GetVehicleMovement()->SetSteeringInput(ActionVector);
}

void ASpeedVehiclePawn::Throttle(const FInputActionInstance& ActionInstance)
{
	if (CurrentBombStatus == BombStatus::Inactive)
		return;
	float ActionVector = ActionInstance.GetValue().Get<float>();
	GetVehicleMovement()->SetThrottleInput(ActionVector);
}

void ASpeedVehiclePawn::Brake(const FInputActionInstance& ActionInstance)
{
	if (CurrentBombStatus == BombStatus::Inactive)
		return;
	float ActionVector = ActionInstance.GetValue().Get<float>();
	GetVehicleMovement()->SetBrakeInput(ActionVector);
}

void ASpeedVehiclePawn::HandBrake(const FInputActionInstance& ActionInstance)
{
	if (CurrentBombStatus == BombStatus::Inactive)
		return;
	bool ActionVector = ActionInstance.GetValue().Get<bool>();
	GetVehicleMovement()->SetHandbrakeInput(ActionVector);
}


void ASpeedVehiclePawn::Look(const FInputActionInstance& ActionInstance)
{
	if (CurrentCameraStatus == CameraStatus::FirstPerson)
		return;

	TimePassedSinceCameraInput = 0.f;
	if(CurrentCameraStatus != CameraStatus::InMenu)
		SwitchCameraStatusTo(CameraStatus::Manual);

	FVector2D LookVector = ActionInstance.GetValue().Get<FVector2D>();

	AddControllerPitchInput((float)LookVector.Y * -1);
	AddControllerYawInput((float)LookVector.X);
}

void ASpeedVehiclePawn::CameraDistance(const FInputActionInstance& ActionInstance)
{
	if (CameraArmComp->TargetArmLength >= 2500.f)
	{
		SwitchCameraStatusTo(CameraStatus::FirstPerson);
		CameraArmComp->TargetArmLength = -500.f;
	}
	else if (CurrentCameraStatus == CameraStatus::FirstPerson)
	{
		SwitchCameraStatusTo(CameraStatus::Follow);
		CameraArmComp->TargetArmLength = 1500.f;
	}
	else
		CameraArmComp->TargetArmLength += 500.f;
			
}