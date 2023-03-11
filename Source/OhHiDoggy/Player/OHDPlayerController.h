// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CommonPlayerController.h"
#include "OhHiDoggy/Camera/OHDCameraAssistInterface.h"
#include "OHDPlayerController.generated.h"

class UOHDSettingsShared;
class AOHDPlayerState;
class UOHDAbilitySystemComponent;
//class AOHDHUD;
class APawn;

UCLASS(Config = Game, Meta = (ShortTooltip = "The base player controller class used by this project."))
class OHHIDOGGY_API AOHDPlayerController : public ACommonPlayerController, public IOHDCameraAssistInterface//, public IOHDTeamAgentInterface
{
	GENERATED_BODY()

public:

	AOHDPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "OHD|PlayerController")
	AOHDPlayerState* GetOHDPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "OHD|PlayerController")
	UOHDAbilitySystemComponent* GetOHDAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "OHD|PlayerController")
	//AOHDHUD* GetOHDHUD() const;
	AHUD* GetOHDHUD() {return GetHUD();};

	// Run a cheat command on the server.
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCheat(const FString& Msg);

	// Run a cheat command on the server for all players.
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCheatAll(const FString& Msg);

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor interface

	//~AController interface
	virtual void OnUnPossess() override;
	//~End of AController interface

	//~APlayerController interface
	virtual void ReceivedPlayer() override;
	virtual void PlayerTick(float DeltaTime) override;
	//~End of APlayerController interface

	//~IOHDCameraAssistInterface interface
	virtual void OnCameraPenetratingTarget() override;
	//~End of IOHDCameraAssistInterface interface

	//~ACommonPlayerController interface
	virtual void OnPossess(APawn* InPawn) override;
	//~End of ACommonPlayerController interface
	
	//~IOHDTeamAgentInterface interface
	// virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	// virtual FGenericTeamId GetGenericTeamId() const override;
	// virtual FOnOHDTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of IOHDTeamAgentInterface interface

	UFUNCTION(BlueprintCallable, Category = "OHD|Character")
	void SetIsAutoRunning(const bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "OHD|Character")
	bool GetIsAutoRunning() const;

private:
	// UPROPERTY()
	// FOnOHDTeamIndexChangedDelegate OnTeamChangedDelegate;

	UPROPERTY()
	APlayerState* LastSeenPlayerState;

// private:
// 	UFUNCTION()
// 	void OnPlayerStateChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

protected:
	// Called when the player state is set or cleared
	virtual void OnPlayerStateChanged();

private:
	void BroadcastOnPlayerStateChanged();

protected:
	//~AController interface
	virtual void InitPlayerState() override;
	virtual void CleanupPlayerState() override;
	virtual void OnRep_PlayerState() override;
	//~End of AController interface

	//~APlayerController interface
	virtual void SetPlayer(UPlayer* InPlayer) override;
	virtual void AddCheats(bool bForce) override;

	virtual void UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId) override;
	virtual void UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;

	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~End of APlayerController interface

	void OnSettingsChanged(UOHDSettingsShared* Settings);
	
	void OnStartAutoRun();
	void OnEndAutoRun();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartAutoRun"))
	void K2_OnStartAutoRun();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnEndAutoRun"))
	void K2_OnEndAutoRun();

	bool bHideViewTargetPawnNextFrame = false;
};


// A player controller used for replay capture and playback
UCLASS()
class AOHDReplayPlayerController : public AOHDPlayerController
{
	GENERATED_BODY()

	virtual void SetPlayer(UPlayer* InPlayer) override;
};
