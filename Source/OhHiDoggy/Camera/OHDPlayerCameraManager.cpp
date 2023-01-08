// Copyright Epic Games, Inc. All Rights Reserved.

#include "OHDPlayerCameraManager.h"
#include "OHDCameraComponent.h"
#include "Engine/Canvas.h"
#include "OHDUICameraManagerComponent.h"
#include "GameFramework/PlayerController.h"

static FName UICameraComponentName(TEXT("UICamera"));

AOHDPlayerCameraManager::AOHDPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultFOV = OHD_CAMERA_DEFAULT_FOV;
	ViewPitchMin = OHD_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = OHD_CAMERA_DEFAULT_PITCH_MAX;

	UICamera = CreateDefaultSubobject<UOHDUICameraManagerComponent>(UICameraComponentName);
}

UOHDUICameraManagerComponent* AOHDPlayerCameraManager::GetUICameraComponent() const
{
	return UICamera;
}

void AOHDPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	// If the UI Camera is looking at something, let it have priority.
	if (UICamera->NeedsToUpdateViewTarget())
	{
		Super::UpdateViewTarget(OutVT, DeltaTime);
		UICamera->UpdateViewTarget(OutVT, DeltaTime);
		return;
	}

	Super::UpdateViewTarget(OutVT, DeltaTime);
}

void AOHDPlayerCameraManager::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	check(Canvas);

	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	DisplayDebugManager.SetFont(GEngine->GetSmallFont());
	DisplayDebugManager.SetDrawColor(FColor::Yellow);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("OHDPlayerCameraManager: %s"), *GetNameSafe(this)));

	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	const APawn* Pawn = (PCOwner ? PCOwner->GetPawn() : nullptr);

	if (const UOHDCameraComponent* CameraComponent = UOHDCameraComponent::FindCameraComponent(Pawn))
	{
		CameraComponent->DrawDebug(Canvas);
	}
}