// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Camera/PlayerCameraManager.h"

#include "OHDUICameraManagerComponent.generated.h"

class APlayerController;
class AOHDPlayerCameraManager;
class AHUD;
class UCanvas;

UCLASS( Transient, Within=OHDPlayerCameraManager )
class UOHDUICameraManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	static UOHDUICameraManagerComponent* GetComponent(APlayerController* PC);

public:
	UOHDUICameraManagerComponent();	
	virtual void InitializeComponent() override;

	bool IsSettingViewTarget() const { return bUpdatingViewTarget; }
	AActor* GetViewTarget() const { return ViewTarget; }
	void SetViewTarget(AActor* InViewTarget, FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams());

	bool NeedsToUpdateViewTarget() const;
	void UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime);

	void OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos);

private:
	UPROPERTY(Transient)
	AActor* ViewTarget;
	
	UPROPERTY(Transient)
	bool bUpdatingViewTarget;
};