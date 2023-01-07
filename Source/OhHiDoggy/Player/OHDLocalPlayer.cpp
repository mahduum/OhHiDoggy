// Fill out your copyright notice in the Description page of Project Settings.
// Copyright Epic Games, Inc. All Rights Reserved.

#include "OHDLocalPlayer.h"
#include "../Settings/OHDSettingsLocal.h"
#include "../Settings/OHDSettingsShared.h"
#include "../Input/OHDInputConfig.h"
#include "InputMappingContext.h"
#include "AudioMixerBlueprintLibrary.h"
#include "GameFramework/PlayerController.h"

UOHDLocalPlayer::UOHDLocalPlayer()
{
}

void UOHDLocalPlayer::PostInitProperties()
{
	Super::PostInitProperties();

	if (UOHDSettingsLocal* LocalSettings = GetLocalSettings())
	{
		LocalSettings->OnAudioOutputDeviceChanged.AddUObject(this, &UOHDLocalPlayer::OnAudioOutputDeviceChanged);
	}
}

void UOHDLocalPlayer::SwitchController(class APlayerController* PC)
{
	Super::SwitchController(PC);

	//OnPlayerControllerChanged(PlayerController);todo
}

bool UOHDLocalPlayer::SpawnPlayActor(const FString& URL, FString& OutError, UWorld* InWorld)
{
	const bool bResult = Super::SpawnPlayActor(URL, OutError, InWorld);

	//OnPlayerControllerChanged(PlayerController);todo

	return bResult;
}

void UOHDLocalPlayer::InitOnlineSession()
{
	//OnPlayerControllerChanged(PlayerController);todo

	Super::InitOnlineSession();
}

// void UOHDLocalPlayer::OnPlayerControllerChanged(APlayerController* NewController)
// {
// 	// Stop listening for changes from the old controller
// 	FGenericTeamId OldTeamID = FGenericTeamId::NoTeam;
// 	if (IOHDTeamAgentInterface* ControllerAsTeamProvider = Cast<IOHDTeamAgentInterface>(LastBoundPC.Get()))
// 	{
// 		OldTeamID = ControllerAsTeamProvider->GetGenericTeamId();
// 		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().RemoveAll(this);
// 	}
//
// 	// Grab the current team ID and listen for future changes
// 	FGenericTeamId NewTeamID = FGenericTeamId::NoTeam;
// 	if (IOHDTeamAgentInterface* ControllerAsTeamProvider = Cast<IOHDTeamAgentInterface>(NewController))
// 	{
// 		NewTeamID = ControllerAsTeamProvider->GetGenericTeamId();
// 		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnControllerChangedTeam);
// 		LastBoundPC = NewController;
// 	}
//
// 	ConditionalBroadcastTeamChanged(this, OldTeamID, NewTeamID);
// }

// void UOHDLocalPlayer::SetGenericTeamId(const FGenericTeamId& NewTeamID)
// {
// 	// Do nothing, we merely observe the team of our associated player controller
// }
//
// FGenericTeamId UOHDLocalPlayer::GetGenericTeamId() const
// {
// 	if (IOHDTeamAgentInterface* ControllerAsTeamProvider = Cast<IOHDTeamAgentInterface>(PlayerController))
// 	{
// 		return ControllerAsTeamProvider->GetGenericTeamId();
// 	}
// 	else
// 	{
// 		return FGenericTeamId::NoTeam;
// 	}
// }

// FOnOHDTeamIndexChangedDelegate* UOHDLocalPlayer::GetOnTeamIndexChangedDelegate()
// {
// 	return &OnTeamChangedDelegate;
//}

UOHDSettingsLocal* UOHDLocalPlayer::GetLocalSettings() const
{
	return UOHDSettingsLocal::Get();
}

UOHDSettingsShared* UOHDLocalPlayer::GetSharedSettings() const
{
	if (!SharedSettings)
	{
		SharedSettings = UOHDSettingsShared::LoadOrCreateSettings(this);
	}

	return SharedSettings;
}

void UOHDLocalPlayer::OnAudioOutputDeviceChanged(const FString& InAudioOutputDeviceId)
{
	FOnCompletedDeviceSwap DevicesSwappedCallback;
	DevicesSwappedCallback.BindUFunction(this, FName("OnCompletedAudioDeviceSwap"));
	UAudioMixerBlueprintLibrary::SwapAudioOutputDevice(GetWorld(), InAudioOutputDeviceId, DevicesSwappedCallback);
}

void UOHDLocalPlayer::OnCompletedAudioDeviceSwap(const FSwapAudioOutputResult& SwapResult)
{
	if (SwapResult.Result == ESwapAudioOutputDeviceResultState::Failure)
	{
	}
}

// void UOHDLocalPlayer::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
// {
// 	ConditionalBroadcastTeamChanged(this, IntegerToGenericTeamId(OldTeam), IntegerToGenericTeamId(NewTeam));
// }