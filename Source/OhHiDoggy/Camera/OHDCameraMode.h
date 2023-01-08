// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "OHDCameraMode.generated.h"


class UOHDCameraComponent;
class UCanvas;

/**
 * EOHDCameraModeBlendFunction
 *
 *	Blend function used for transitioning between camera modes.
 */
UENUM(BlueprintType)
enum class EOHDCameraModeBlendFunction : uint8
{
	// Does a simple linear interpolation.
	Linear,

	// Immediately accelerates, but smoothly decelerates into the target.  Ease amount controlled by the exponent.
	EaseIn,

	// Smoothly accelerates, but does not decelerate into the target.  Ease amount controlled by the exponent.
	EaseOut,

	// Smoothly accelerates and decelerates.  Ease amount controlled by the exponent.
	EaseInOut,

	COUNT	UMETA(Hidden)
};


/**
 * FOHDCameraModeView
 *
 *	View data produced by the camera mode that is used to blend camera modes.
 */
struct FOHDCameraModeView
{
public:

	FOHDCameraModeView();

	void Blend(const FOHDCameraModeView& Other, float OtherWeight);

public:

	FVector Location;
	FRotator Rotation;
	FRotator ControlRotation;
	float FieldOfView;
};


/**
 * UOHDCameraMode
 *
 *	Base class for all camera modes.
 */
UCLASS(Abstract, NotBlueprintable)
class OHHIDOGGY_API UOHDCameraMode : public UObject
{
	GENERATED_BODY()

public:

	UOHDCameraMode();

	UOHDCameraComponent* GetOHDCameraComponent() const;

	virtual UWorld* GetWorld() const override;

	AActor* GetTargetActor() const;

	const FOHDCameraModeView& GetCameraModeView() const { return View; }

	// Called when this camera mode is activated on the camera mode stack.
	virtual void OnActivation() {};

	// Called when this camera mode is deactivated on the camera mode stack.
	virtual void OnDeactivation() {};

	void UpdateCameraMode(float DeltaTime);

	float GetBlendTime() const { return BlendTime; }
	float GetBlendWeight() const { return BlendWeight; }
	void SetBlendWeight(float Weight);

	FGameplayTag GetCameraTypeTag() const
	{
		return CameraTypeTag;
	}

	virtual void DrawDebug(UCanvas* Canvas) const;

protected:

	virtual FVector GetPivotLocation() const;
	virtual FRotator GetPivotRotation() const;

	virtual void UpdateView(float DeltaTime);
	virtual void UpdateBlending(float DeltaTime);

protected:
	// A tag that can be queried by gameplay code that cares when a kind of camera mode is active
	// without having to ask about a specific mode (e.g., when aiming downsights to get more accuracy)
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	FGameplayTag CameraTypeTag;

	// View output produced by the camera mode.
	FOHDCameraModeView View;

	// The horizontal field of view (in degrees).
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", ClampMax = "170.0"))
	float FieldOfView;

	// Minimum view pitch (in degrees).
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMin;

	// Maximum view pitch (in degrees).
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMax;

	// How long it takes to blend in this mode.
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime;

	// Function used for blending.
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	EOHDCameraModeBlendFunction BlendFunction;

	// Exponent used by blend functions to control the shape of the curve.
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendExponent;

	// Linear blend alpha used to determine the blend weight.
	float BlendAlpha;

	// Blend weight calculated using the blend alpha and function.
	float BlendWeight;

protected:
	/** If true, skips all interpolation and puts camera in ideal location.  Automatically set to false next frame. */
	UPROPERTY(transient)
	uint32 bResetInterpolation:1;
};


/**
 * UOHDCameraModeStack
 *
 *	Stack used for blending camera modes.
 */
UCLASS()
class UOHDCameraModeStack : public UObject
{
	GENERATED_BODY()

public:

	UOHDCameraModeStack();

	void ActivateStack();
	void DeactivateStack();

	bool IsStackActivate() const { return bIsActive; }

	void PushCameraMode(TSubclassOf<UOHDCameraMode> CameraModeClass);

	bool EvaluateStack(float DeltaTime, FOHDCameraModeView& OutCameraModeView);

	void DrawDebug(UCanvas* Canvas) const;

	// Gets the tag associated with the top layer and the blend weight of it
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;

protected:

	UOHDCameraMode* GetCameraModeInstance(TSubclassOf<UOHDCameraMode> CameraModeClass);

	void UpdateStack(float DeltaTime);
	void BlendStack(FOHDCameraModeView& OutCameraModeView) const;

protected:

	bool bIsActive;

	UPROPERTY()
	TArray<UOHDCameraMode*> CameraModeInstances;

	UPROPERTY()
	TArray<UOHDCameraMode*> CameraModeStack;
};

