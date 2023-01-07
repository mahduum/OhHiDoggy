// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Engine/DataAsset.h"
#include "../Settings/OHDSettingsShared.h"
#include "OHDAimSensitivityData.generated.h"

/**
 * 
 */
/** Defines a set of gamepad sensitivity to a float value. */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Lyra Aim Sensitivity Data", ShortTooltip = "Data asset used to define a map of Gamepad Sensitivty to a float value."))
class OHHIDOGGY_API UOHDAimSensitivityData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UOHDAimSensitivityData(const FObjectInitializer& ObjectInitializer);
	
	const float SensitivtyEnumToFloat(const EOHDGamepadSensitivity InSensitivity) const;
	
protected:
	/** Map of SensitivityMap settings to their corresponding float */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EOHDGamepadSensitivity, float> SensitivityMap;
};
