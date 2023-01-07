// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonLocalPlayer.h"
#include "AudioMixerBlueprintLibrary.h"
//#include "Teams/OHDTeamAgentInterface.h"
#include "OHDLocalPlayer.generated.h"

class UOHDSettingsLocal;
class UOHDSettingsShared;
class UInputMappingContext;

/**
 * UOHDLocalPlayer
 */
UCLASS()
class OHHIDOGGY_API UOHDLocalPlayer : public UCommonLocalPlayer //public IOHDTeamAgentInterface
{
	GENERATED_BODY()

public:

	UOHDLocalPlayer();

	//~UObject interface
	virtual void PostInitProperties() override;
	//~End of UObject interface

	//~UPlayer interface
	virtual void SwitchController(class APlayerController* PC) override;
	//~End of UPlayer interface

	//~ULocalPlayer interface
	virtual bool SpawnPlayActor(const FString& URL, FString& OutError, UWorld* InWorld) override;
	virtual void InitOnlineSession() override;
	//~End of ULocalPlayer interface

	//~IOHDTeamAgentInterface interface //todo
	//virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	//virtual FGenericTeamId GetGenericTeamId() const override;
	//virtual FOnOHDTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of IOHDTeamAgentInterface interface

	public:
	UFUNCTION()
	UOHDSettingsLocal* GetLocalSettings() const;

	UFUNCTION()
	UOHDSettingsShared* GetSharedSettings() const;

protected:
	void OnAudioOutputDeviceChanged(const FString& InAudioOutputDeviceId);
	
	UFUNCTION()
	void OnCompletedAudioDeviceSwap(const FSwapAudioOutputResult& SwapResult);

// private:
// 	void OnPlayerControllerChanged(APlayerController* NewController);todo
//
// 	UFUNCTION()
// 	void OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);todo

private:
	UPROPERTY(Transient)
	mutable UOHDSettingsShared* SharedSettings;

	UPROPERTY(Transient)
	mutable const UInputMappingContext* InputMappingContext;

	// UPROPERTY()
	// FOnOHDTeamIndexChangedDelegate OnTeamChangedDelegate;todo

	UPROPERTY()
	TWeakObjectPtr<APlayerController> LastBoundPC;
};
