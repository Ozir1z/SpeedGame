// Fill out your copyright notice in the Description page of Project Settings.

#include "SpeedVehiclePawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include <EnhancedInputSubsystems.h>
#include <ChaosVehicleMovementComponent.h>
#include <EnhancedInputComponent.h>
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "SpeedGameGameModeBase.h"
//#include "SpeedGameInstance.h"

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
			PlayerEnhancedInputComponent->BindAction(HandBrakeAction, ETriggerEvent::Triggered, this, &ASpeedVehiclePawn::HandBrake);
			PlayerEnhancedInputComponent->BindAction(HandBrakeAction, ETriggerEvent::Completed, this, &ASpeedVehiclePawn::HandBrake);
		}
		if (LookAction)
		{
			PlayerEnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASpeedVehiclePawn::Look);
		}
	}	
}

void ASpeedVehiclePawn::BeginPlay()
{
	Super::BeginPlay();
}


void ASpeedVehiclePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleVehicleSpeed();
	
	TimePassedSinceCameraInput += DeltaTime;
	if (TimePassedSinceCameraInput > 1.5f)
		SwitchCameraStatusTo(CameraStatus::Follow);
}

void ASpeedVehiclePawn::HandleVehicleSpeed()
{
	//USpeedGameInstance* gameInstance = Cast<USpeedGameInstance>(GetWorld()->GetGameInstance());
	ASpeedGameGameModeBase* gameMode = (ASpeedGameGameModeBase*)GetWorld()->GetAuthGameMode();

	int speed = static_cast<int>(GetVehicleMovementComponent()->GetForwardSpeedMPH());
	if (speed <= 0) // make a maxspeed variable in gamemode, so we can make difficulties if we want to
		speed = 0;
	if (speed >= 30 && CurrentBombStatus == BombStatus::Inactive)
		CurrentBombStatus = BombStatus::Active;
	if (speed < 30 && CurrentBombStatus == BombStatus::Active)
	{
		CurrentBombStatus = BombStatus::Explodeded;
		Death();
	}
	if (CurrentBombStatus == BombStatus::Explodeded)
	{
		speed = 0;
		gameMode->StopGame();
	}

	gameMode->UpdateSpeed(speed);
}

void ASpeedVehiclePawn::Death()
{
	GetMesh()->SetSkeletalMeshAsset(SecondaryMesh);
	GetMesh()->AddVelocityChangeImpulseAtLocation(FVector(200,0, 200), FVector(), TEXT("Body"));
	

	if(DeathParticle)
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(DeathParticle, RootComponent, NAME_None, FVector(0.f), FRotator(0.f), EAttachLocation::Type::KeepRelativeOffset, true);
}

void ASpeedVehiclePawn::SwitchCameraStatusTo(CameraStatus newCameraStatus)
{
	CurrentCameraStatus = newCameraStatus;
	switch (CurrentCameraStatus) 
	{
		case CameraStatus::Manual:
			CameraArmComp->bUsePawnControlRotation = true;
			break;
		case CameraStatus::Follow:
			GetController()->ClientSetRotation(CameraArmComp->GetComponentRotation());
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
		float ActionVector = ActionInstance.GetValue().Get<float>();
		GetVehicleMovement()->SetThrottleInput(ActionVector);
}

void ASpeedVehiclePawn::Brake(const FInputActionInstance& ActionInstance)
{
		float ActionVector = ActionInstance.GetValue().Get<float>();
		GetVehicleMovement()->SetBrakeInput(ActionVector);
}

void ASpeedVehiclePawn::HandBrake(const FInputActionInstance& ActionInstance)
{
		bool ActionVector = ActionInstance.GetValue().Get<bool>();
		GetVehicleMovement()->SetHandbrakeInput(ActionVector);
}


void ASpeedVehiclePawn::Look(const FInputActionInstance& ActionInstance)
{
	TimePassedSinceCameraInput = 0.f;
	SwitchCameraStatusTo(CameraStatus::Manual);

	FVector2D LookVector = ActionInstance.GetValue().Get<FVector2D>();

	AddControllerPitchInput((float)LookVector.Y * -1);
	AddControllerYawInput((float)LookVector.X);
}
