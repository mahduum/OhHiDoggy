// Fill out your copyright notice in the Description page of Project Settings.


#include "OHDSettingsShared.h"


#include "OHDSettingsShared.h"

#include "SubtitleDisplaySubsystem.h"
#include "Framework/Application/SlateApplication.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "../Player/OHDLocalPlayer.h"
#include "Internationalization/Culture.h"

static FString SHARED_SETTINGS_SLOT_NAME = TEXT("SharedGameSettings");

namespace OHDSettingsSharedCVars
{
	static float DefaultGamepadLeftStickInnerDeadZone = 0.25f;
	static FAutoConsoleVariableRef CVarGamepadLeftStickInnerDeadZone(
		TEXT("gpad.DefaultLeftStickInnerDeadZone"),
		DefaultGamepadLeftStickInnerDeadZone,
		TEXT("Gamepad left stick inner deadzone")
	);

	static float DefaultGamepadRightStickInnerDeadZone = 0.25f;
	static FAutoConsoleVariableRef CVarGamepadRightStickInnerDeadZone(
		TEXT("gpad.DefaultRightStickInnerDeadZone"),
		DefaultGamepadRightStickInnerDeadZone,
		TEXT("Gamepad right stick inner deadzone")
	);	
}

UOHDSettingsShared::UOHDSettingsShared()
{
	FInternationalization::Get().OnCultureChanged().AddUObject(this, &ThisClass::OnCultureChanged);

	GamepadMoveStickDeadZone = OHDSettingsSharedCVars::DefaultGamepadLeftStickInnerDeadZone;
	GamepadLookStickDeadZone = OHDSettingsSharedCVars::DefaultGamepadRightStickInnerDeadZone;
}

void UOHDSettingsShared::Initialize(UOHDLocalPlayer* LocalPlayer)
{
	check(LocalPlayer);
	
	OwningPlayer = LocalPlayer;
}

void UOHDSettingsShared::SaveSettings()
{
	check(OwningPlayer);
	UGameplayStatics::SaveGameToSlot(this, SHARED_SETTINGS_SLOT_NAME, OwningPlayer->GetLocalPlayerIndex());
}

/*static*/ UOHDSettingsShared* UOHDSettingsShared::LoadOrCreateSettings(const UOHDLocalPlayer* LocalPlayer)
{
	UOHDSettingsShared* SharedSettings = nullptr;

	// If the save game exists, load it.
	if (UGameplayStatics::DoesSaveGameExist(SHARED_SETTINGS_SLOT_NAME, LocalPlayer->GetLocalPlayerIndex()))
	{
		USaveGame* Slot = UGameplayStatics::LoadGameFromSlot(SHARED_SETTINGS_SLOT_NAME, LocalPlayer->GetLocalPlayerIndex());
		SharedSettings = Cast<UOHDSettingsShared>(Slot);
	}
	
	if (SharedSettings == nullptr)
	{
		SharedSettings = Cast<UOHDSettingsShared>(UGameplayStatics::CreateSaveGameObject(UOHDSettingsShared::StaticClass()));
	}

	SharedSettings->Initialize(const_cast<UOHDLocalPlayer*>(LocalPlayer));
	SharedSettings->ApplySettings();

	return SharedSettings;
}

void UOHDSettingsShared::ApplySettings()
{
	ApplySubtitleOptions();
	ApplyBackgroundAudioSettings();
	ApplyCultureSettings();
}

void UOHDSettingsShared::SetColorBlindStrength(int32 InColorBlindStrength)
{
	InColorBlindStrength = FMath::Clamp(InColorBlindStrength, 0, 10);
	if (ColorBlindStrength != InColorBlindStrength)
	{
		ColorBlindStrength = InColorBlindStrength;
		FSlateApplication::Get().GetRenderer()->SetColorVisionDeficiencyType(
			(EColorVisionDeficiency)(int32)ColorBlindMode, (int32)ColorBlindStrength, true, false);
	}
}

int32 UOHDSettingsShared::GetColorBlindStrength() const
{
	return ColorBlindStrength;
}

void UOHDSettingsShared::SetColorBlindMode(EColorBlindMode InMode)
{
	if (ColorBlindMode != InMode)
	{
		ColorBlindMode = InMode;
		FSlateApplication::Get().GetRenderer()->SetColorVisionDeficiencyType(
			(EColorVisionDeficiency)(int32)ColorBlindMode, (int32)ColorBlindStrength, true, false);
	}
}

EColorBlindMode UOHDSettingsShared::GetColorBlindMode() const
{
	return ColorBlindMode;
}

void UOHDSettingsShared::ApplySubtitleOptions()
{
	if (USubtitleDisplaySubsystem* SubtitleSystem = USubtitleDisplaySubsystem::Get(OwningPlayer))
	{
		FSubtitleFormat SubtitleFormat;
		SubtitleFormat.SubtitleTextSize = SubtitleTextSize;
		SubtitleFormat.SubtitleTextColor = SubtitleTextColor;
		SubtitleFormat.SubtitleTextBorder = SubtitleTextBorder;
		SubtitleFormat.SubtitleBackgroundOpacity = SubtitleBackgroundOpacity;

		SubtitleSystem->SetSubtitleDisplayOptions(SubtitleFormat);
	}
}

//////////////////////////////////////////////////////////////////////

void UOHDSettingsShared::SetAllowAudioInBackgroundSetting(EOHDAllowBackgroundAudioSetting NewValue)
{
	if (ChangeValueAndDirty(AllowAudioInBackground, NewValue))
	{
		ApplyBackgroundAudioSettings();
	}
}

void UOHDSettingsShared::ApplyBackgroundAudioSettings()
{
	if (OwningPlayer && OwningPlayer->IsPrimaryPlayer())
	{
		FApp::SetUnfocusedVolumeMultiplier((AllowAudioInBackground != EOHDAllowBackgroundAudioSetting::Off) ? 1.0f : 0.0f);
	}
}

//////////////////////////////////////////////////////////////////////

void UOHDSettingsShared::ApplyCultureSettings()
{
	if (bResetToDefaultCulture)
	{
		const FCulturePtr SystemDefaultCulture = FInternationalization::Get().GetDefaultCulture();
		check(SystemDefaultCulture.IsValid());

		const FString CultureToApply = SystemDefaultCulture->GetName();
		if (FInternationalization::Get().SetCurrentCulture(CultureToApply))
		{
			// Clear this string
			GConfig->RemoveKey(TEXT("Internationalization"), TEXT("Culture"), GGameUserSettingsIni);
			GConfig->Flush(false, GGameUserSettingsIni);
		}
		bResetToDefaultCulture = false;
	}
	else if (!PendingCulture.IsEmpty())
	{
		// SetCurrentCulture may trigger PendingCulture to be cleared (if a culture change is broadcast) so we take a copy of it to work with
		const FString CultureToApply = PendingCulture;
		if (FInternationalization::Get().SetCurrentCulture(CultureToApply))
		{
			// Note: This is intentionally saved to the users config
			// We need to localize text before the player logs in and very early in the loading screen
			GConfig->SetString(TEXT("Internationalization"), TEXT("Culture"), *CultureToApply, GGameUserSettingsIni);
			GConfig->Flush(false, GGameUserSettingsIni);
		}
		ClearPendingCulture();
	}
}

void UOHDSettingsShared::ResetCultureToCurrentSettings()
{
	ClearPendingCulture();
	bResetToDefaultCulture = false;
}

const FString& UOHDSettingsShared::GetPendingCulture() const
{
	return PendingCulture;
}

void UOHDSettingsShared::SetPendingCulture(const FString& NewCulture)
{
	PendingCulture = NewCulture;
	bResetToDefaultCulture = false;
	bIsDirty = true;
}

void UOHDSettingsShared::OnCultureChanged()
{
	ClearPendingCulture();
	bResetToDefaultCulture = false;
}

void UOHDSettingsShared::ClearPendingCulture()
{
	PendingCulture.Reset();
}

bool UOHDSettingsShared::IsUsingDefaultCulture() const
{
	FString Culture;
	GConfig->GetString(TEXT("Internationalization"), TEXT("Culture"), Culture, GGameUserSettingsIni);

	return Culture.IsEmpty();
}

void UOHDSettingsShared::ResetToDefaultCulture()
{
	ClearPendingCulture();
	bResetToDefaultCulture = true;
	bIsDirty = true;
}

//////////////////////////////////////////////////////////////////////

void UOHDSettingsShared::ApplyInputSensitivity()
{
	
}

