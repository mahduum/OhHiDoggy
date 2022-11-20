// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OhHiDoggy/Components/OHDPawnComponentExtension.h"
#include "CanineCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDoggyPossessed, UInputComponent*)

USTRUCT()
struct FCanineReplicatedAcceleration
{
	GENERATED_BODY()

	UPROPERTY()
	uint8 AccelXYRadians = 0;	// Direction of XY accel component, quantized to represent [0, 2*pi]

	UPROPERTY()
	uint8 AccelXYMagnitude = 0;	//Accel rate of XY component, quantized to represent [0, MaxAcceleration]

	UPROPERTY()
	int8 AccelZ = 0;	// Raw Z accel rate component, quantized to represent [-MaxAcceleration, MaxAcceleration]
};

class UAbilitySystemComponent;

//TODO: in the future make it derive from AModularCharacter, also copy abilities set, movement tags etc. from LyraCharacter
UCLASS(Config = Game, Meta = (ShortTooltip = "The base character pawn class used by this project."))
class ACanineCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ACanineCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

	//todo use from PawnComponentExt instead
	FOnDoggyPossessed OnDoggyPossessedDelegate;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lyra|Character", Meta = (AllowPrivateAccess = "true"))
	UOHDPawnComponentExtension* PawnExtComponent;
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lyra|Character", Meta = (AllowPrivateAccess = "true"))
	// UCanineCameraComponent* CameraComponent;
	//
};

