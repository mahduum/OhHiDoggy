// Fill out your copyright notice in the Description page of Project Settings.


#include "OHDAimSensitivityData.h"

#include "OhHiDoggy/Settings/OHDSettingsShared.h"

UOHDAimSensitivityData::UOHDAimSensitivityData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SensitivityMap =
	{
		{ EOHDGamepadSensitivity::Slow,			0.5f },
		{ EOHDGamepadSensitivity::SlowPlus,		0.75f },
		{ EOHDGamepadSensitivity::SlowPlusPlus,	0.9f },
		{ EOHDGamepadSensitivity::Normal,		1.0f },
		{ EOHDGamepadSensitivity::NormalPlus,	1.1f },
		{ EOHDGamepadSensitivity::NormalPlusPlus,1.25f },
		{ EOHDGamepadSensitivity::Fast,			1.5f },
		{ EOHDGamepadSensitivity::FastPlus,		1.75f },
		{ EOHDGamepadSensitivity::FastPlusPlus,	2.0f },
		{ EOHDGamepadSensitivity::Insane,		2.5f },
	};
}

const float UOHDAimSensitivityData::SensitivtyEnumToFloat(const EOHDGamepadSensitivity InSensitivity) const
{
	if (const float* Sens = SensitivityMap.Find(InSensitivity))
	{
		return *Sens;
	}

	return 1.0f;
}