// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "OHDCameraComponent.generated.h"


class UOHDCameraMode;
class UOHDCameraModeStack;
class UCanvas;
struct FGameplayTag;

DECLARE_DELEGATE_RetVal(TSubclassOf<UOHDCameraMode>, FOHDCameraModeDelegate);


/**
 * UOHDCameraComponent
 *
 *	The base camera component class used by this project.
 */
UCLASS()
class OHHIDOGGY_API UOHDCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:

	UOHDCameraComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the camera component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "OHD|Camera")
	static UOHDCameraComponent* FindCameraComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UOHDCameraComponent>() : nullptr); }

	// Returns the target actor that the camera is looking at.
	virtual AActor* GetTargetActor() const { return GetOwner(); }

	// Delegate used to query for the best camera mode.
	FOHDCameraModeDelegate DetermineCameraModeDelegate;

	// Add an offset to the field of view.  The offset is only for one frame, it gets cleared once it is applied.
	void AddFieldOfViewOffset(float FovOffset) { FieldOfViewOffset += FovOffset; }

	virtual void DrawDebug(UCanvas* Canvas) const;

	// Gets the tag associated with the top layer and the blend weight of it
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;

protected:

	virtual void OnRegister() override;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

	virtual void UpdateCameraModes();

protected:

	// Stack used to blend the camera modes.
	UPROPERTY()
	UOHDCameraModeStack* CameraModeStack;

	// Offset applied to the field of view.  The offset is only for one frame, it gets cleared once it is applied.
	float FieldOfViewOffset;

};

