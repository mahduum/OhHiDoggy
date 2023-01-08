// Copyright Epic Games, Inc. All Rights Reserved.

#include "OHDUICameraManagerComponent.h"
#include "EngineUtils.h"
#include "Algo/Transform.h"
#include "DrawDebugHelpers.h"
#include "Engine/Canvas.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "Slate/SceneViewport.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "GameFramework/HUD.h"
#include "Engine/Engine.h"

#include "OHDCameraMode.h"
#include "OHDPlayerCameraManager.h"

UOHDUICameraManagerComponent* UOHDUICameraManagerComponent::GetComponent(APlayerController* PC)
{
	if (PC != nullptr)
	{
		if (AOHDPlayerCameraManager* PCCamera = Cast<AOHDPlayerCameraManager>(PC->PlayerCameraManager))
		{
			return PCCamera->GetUICameraComponent();
		}
	}

	return nullptr;
}

UOHDUICameraManagerComponent::UOHDUICameraManagerComponent()
{
	bWantsInitializeComponent = true;

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		// Register "showdebug" hook.
		if (!IsRunningDedicatedServer())
		{
			AHUD::OnShowDebugInfo.AddUObject(this, &ThisClass::OnShowDebugInfo);
		}
	}
}

void UOHDUICameraManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UOHDUICameraManagerComponent::SetViewTarget(AActor* InViewTarget, FViewTargetTransitionParams TransitionParams)
{
	TGuardValue<bool> UpdatingViewTargetGuard(bUpdatingViewTarget, true);

	ViewTarget = InViewTarget;
	CastChecked<AOHDPlayerCameraManager>(GetOwner())->SetViewTarget(ViewTarget, TransitionParams);
}

bool UOHDUICameraManagerComponent::NeedsToUpdateViewTarget() const
{
	return false;
}

void UOHDUICameraManagerComponent::UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime)
{
}

void UOHDUICameraManagerComponent::OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos)
{
}