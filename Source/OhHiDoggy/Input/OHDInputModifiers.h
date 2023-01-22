// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

/**
 * 
 */

#include "InputModifiers.h"
#include "../Settings/OHDSettingsShared.h"
#include "OHDInputModifiers.generated.h"

class UEnhancedPlayerInput;
class UOHDAimSensitivityData;

/** 
*  Scales input basedon a double property in the SharedUserSettings
*/
UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "Setting Based Scalar"))
class UOHDSettingBasedScalar : public UInputModifier
{
	GENERATED_BODY()

public:

	/** Name of the property that will be used to clamp the X Axis of this value */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
	FName XAxisScalarSettingName = NAME_None;

	/** Name of the property that will be used to clamp the Y Axis of this value */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
	FName YAxisScalarSettingName = NAME_None;

	/** Name of the property that will be used to clamp the Z Axis of this value */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
	FName ZAxisScalarSettingName = NAME_None;
	
	/** Set the maximium value of this setting on each axis. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
	FVector MaxValueClamp = FVector(10.0, 10.0, 10.0);
	
	/** Set the minimum value of this setting on each axis. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
	FVector MinValueClamp = FVector::ZeroVector;

protected:
	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime) override;

	/** FProperty Cache that will be populated with any found FProperty's on the settings class so that we don't need to look them up each frame */
	TArray<const FProperty*> PropertyCache;
};

/** Represents which stick that this deadzone is for, either the move or the look stick */
UENUM()
enum class EDeadzoneStick : uint8
{
	/** Deadzone for the movement stick */
	MoveStick = 0,

	/** Deadzone for the looking stick */
	LookStick = 1,
};

/**
 * This is a deadzone input modifier that will have it's thresholds driven by what is in the OHD Shared game settings. 
 */
UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "OHD Settings Driven Dead Zone"))
class UOHDInputModifierDeadZone : public UInputModifier
{
	GENERATED_BODY()

public:

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings, Config)
	EDeadZoneType Type = EDeadZoneType::Radial;
	
	// Threshold above which input is clamped to 1
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings, Config)
	float UpperThreshold = 1.0f;

	/** Which stick this deadzone is for. This controls which setting will be used when calculating the deadzone */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings, Config)
	EDeadzoneStick DeadzoneStick = EDeadzoneStick::MoveStick;

protected:
	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime) override;

	// Visualize as black when unmodified. Red when blocked (with differing intensities to indicate axes)
	// Mirrors visualization in https://www.gamasutra.com/blogs/JoshSutphin/20130416/190541/Doing_Thumbstick_Dead_Zones_Right.php.
	virtual FLinearColor GetVisualizationColor_Implementation(FInputActionValue SampleValue, FInputActionValue FinalValue) const override;
};

/** The type of targeting sensitity that should be  */
UENUM()
enum class EOHDTargetingType : uint8
{
	/** Sensitivity to be applied why normally looking around */
	Normal = 0,

	/** The sensitivity that should be applied while Aiming Down Sights */
	ADS = 1,
};

/** Applies a scalar modifier based on the current gamepad settings in OHD Shared game settings.  */
UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "OHD Gamepad Sensitivity"))
class UOHDInputModifierGamepadSensitivity : public UInputModifier
{
	GENERATED_BODY()
public:
	
	/** The type of targeting to use for this Sensitivity */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings, Config)
	EOHDTargetingType TargetingType = EOHDTargetingType::Normal;

	/** Asset that gives us access to the float scalar value being used for sensitivty */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AssetBundles="Client,Server"))
	TObjectPtr<const UOHDAimSensitivityData> SensitivityLevelTable;

protected:
	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime) override;
};

/** Applies an inversion of axis values based on a setting in the OHD Shared game settings */
UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "OHD Aim Inversion Setting"))
class UOHDInputModifierAimInversion : public UInputModifier
{
	GENERATED_BODY()
	
protected:
	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime) override;	
};

UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "OHD Smooth"))
class UOHDInputModifierSmooth final : public UInputModifier
{
	GENERATED_BODY()

	void ClearSmoothedAxis();

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Settings)
	float TimeDilation = 0.2f;

	/** How long input has been zero. */
	float ZeroTime = 0.f;

	/** Current average input/sample */
	FInputActionValue LastValue;

	/** Number of samples since input  has been zero */
	int32 Samples = 0;

#define SMOOTH_TOTAL_SAMPLE_TIME_DEFAULT (0.0083f)
	/** Input sampling total time. */
	float TotalSampleTime = SMOOTH_TOTAL_SAMPLE_TIME_DEFAULT;

	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime) override;
};

UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "OHD Dampen"))
class UOHDInputModifierDamp final : public UInputModifier
{
	
	GENERATED_BODY()

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Settings)
	float DefaultDampingFactor = 1.0f;

	float RuntimeDampingFactor = 1.0f;

	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime) override;
};