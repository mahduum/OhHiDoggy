// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "OHDPlayerCameraManager.generated.h"


#define OHD_CAMERA_DEFAULT_FOV			(80.0f)
#define OHD_CAMERA_DEFAULT_PITCH_MIN	(-89.0f)
#define OHD_CAMERA_DEFAULT_PITCH_MAX	(89.0f)

class UOHDUICameraManagerComponent;

/**
 * AOHDPlayerCameraManager
 *
 *	The base player camera manager class used by this project.
 */
UCLASS(notplaceable, MinimalAPI)
class AOHDPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:

	AOHDPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

	UOHDUICameraManagerComponent* GetUICameraComponent() const;

protected:

	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;

private:
	/** The UI Camera Component, controls the camera when UI is doing something important that gameplay doesn't get priority over. */
	UPROPERTY(Transient)
	UOHDUICameraManagerComponent* UICamera;
};
