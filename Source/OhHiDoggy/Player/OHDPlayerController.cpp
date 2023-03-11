// Fill out your copyright notice in the Description page of Project Settings.


#include "OHDPlayerController.h"
#include "../OHDLogChannels.h"
#include "../GameModes/OHDGameMode.h"
//#include "OHDCheatManager.h"//todo
#include "OHDPlayerState.h"
#include "../Camera/OHDPlayerCameraManager.h"
//#include "UI/OHDHUD.h"
#include "../Data/OHDPawnData.h"
#include "../AbilitySystem/OHDAbilitySystemComponent.h"
#include "EngineUtils.h"
#include "../FOHDGameplayTags.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemGlobals.h"
#include "CommonInputSubsystem.h"
#include "OHDLocalPlayer.h"
#include "../Settings/OHDSettingsShared.h"
//#include "Development/OHDDeveloperSettings.h"
#include "OhHiDoggy/Camera/OHDPlayerCameraManager.h"

AOHDPlayerController::AOHDPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = AOHDPlayerCameraManager::StaticClass();
//
// #if USING_CHEAT_MANAGER
// 	CheatClass = UOHDCheatManager::StaticClass();//todo cheatmanager
// #endif // #if USING_CHEAT_MANAGER
}

void AOHDPlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AOHDPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(false);
}

void AOHDPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AOHDPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
}

void AOHDPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// If we are auto running then add some player input
	if (GetIsAutoRunning())
	{
		if (APawn* CurrentPawn = GetPawn())
		{
			const FRotator MovementRotation(0.0f, GetControlRotation().Yaw, 0.0f);
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			CurrentPawn->AddMovementInput(MovementDirection, 1.0f);	
		}
	}
}

AOHDPlayerState* AOHDPlayerController::GetOHDPlayerState() const
{
	return CastChecked<AOHDPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UOHDAbilitySystemComponent* AOHDPlayerController::GetOHDAbilitySystemComponent() const
{
	const AOHDPlayerState* OHDPS = GetOHDPlayerState();
	return (OHDPS ? OHDPS->GetOHDAbilitySystemComponent() : nullptr);
}
//
// AOHDHUD* AOHDPlayerController::GetOHDHUD() const
// {
// 	return CastChecked<AOHDHUD>(GetHUD(), ECastCheckedType::NullAllowed);
// }
//
// void AOHDPlayerController::OnPlayerStateChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
// {
// 	ConditionalBroadcastTeamChanged(this, IntegerToGenericTeamId(OldTeam), IntegerToGenericTeamId(NewTeam));
// }

void AOHDPlayerController::OnPlayerStateChanged()
{
	// Empty, place for derived classes to implement without having to hook all the other events
}

void AOHDPlayerController::BroadcastOnPlayerStateChanged()
{
	OnPlayerStateChanged();

	// Unbind from the old player state, if any //todo team secondary
	// FGenericTeamId OldTeamID = FGenericTeamId::NoTeam;
	// if (LastSeenPlayerState != nullptr)
	// {
	// 	if (IOHDTeamAgentInterface* PlayerStateTeamInterface = Cast<IOHDTeamAgentInterface>(LastSeenPlayerState))
	// 	{
	// 		OldTeamID = PlayerStateTeamInterface->GetGenericTeamId();
	// 		PlayerStateTeamInterface->GetTeamChangedDelegateChecked().RemoveAll(this);
	// 	}
	// }
	//
	// // Bind to the new player state, if any
	// FGenericTeamId NewTeamID = FGenericTeamId::NoTeam;
	// if (PlayerState != nullptr)
	// {
	// 	if (IOHDTeamAgentInterface* PlayerStateTeamInterface = Cast<IOHDTeamAgentInterface>(PlayerState))
	// 	{
	// 		NewTeamID = PlayerStateTeamInterface->GetGenericTeamId();
	// 		PlayerStateTeamInterface->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnPlayerStateChangedTeam);
	// 	}
	// }
	//
	// // Broadcast the team change (if it really has)
	// ConditionalBroadcastTeamChanged(this, OldTeamID, NewTeamID);

	LastSeenPlayerState = PlayerState;
}

void AOHDPlayerController::InitPlayerState()
{
	Super::InitPlayerState();
	BroadcastOnPlayerStateChanged();
}

void AOHDPlayerController::CleanupPlayerState()
{
	Super::CleanupPlayerState();
	BroadcastOnPlayerStateChanged();
}

void AOHDPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	BroadcastOnPlayerStateChanged();
}

void AOHDPlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);

	if (const UOHDLocalPlayer* OHDLocalPlayer = Cast<UOHDLocalPlayer>(InPlayer))
	{
		UOHDSettingsShared* UserSettings = OHDLocalPlayer->GetSharedSettings();
		UserSettings->OnSettingChanged.AddUObject(this, &ThisClass::OnSettingsChanged);

		OnSettingsChanged(UserSettings);
	}
}

void AOHDPlayerController::OnSettingsChanged(UOHDSettingsShared* InSettings)
{
	bForceFeedbackEnabled = InSettings->GetForceFeedbackEnabled();
}

void AOHDPlayerController::AddCheats(bool bForce)
{
// #if USING_CHEAT_MANAGER//todo cheatmanager
// 	Super::AddCheats(true);
// #else //#if USING_CHEAT_MANAGER
// 	Super::AddCheats(bForce);
// #endif // #else //#if USING_CHEAT_MANAGER
}

void AOHDPlayerController::ServerCheat_Implementation(const FString& Msg)
{
// #if USING_CHEAT_MANAGER//todo cheatmanager
// 	if (CheatManager)
// 	{
// 		UE_LOG(LogOHD, Warning, TEXT("ServerCheat: %s"), *Msg);
// 		ClientMessage(ConsoleCommand(Msg));
// 	}
// #endif // #if USING_CHEAT_MANAGER
}

bool AOHDPlayerController::ServerCheat_Validate(const FString& Msg)
{
	return true;
}

void AOHDPlayerController::ServerCheatAll_Implementation(const FString& Msg)
{
// #if USING_CHEAT_MANAGER//todo cheatmanager
// 	if (CheatManager)
// 	{
// 		UE_LOG(LogOHD, Warning, TEXT("ServerCheatAll: %s"), *Msg);
// 		for (TActorIterator<AOHDPlayerController> It(GetWorld()); It; ++It)
// 		{
// 			AOHDPlayerController* OHDPC = (*It);
// 			if (OHDPC)
// 			{
// 				OHDPC->ClientMessage(OHDPC->ConsoleCommand(Msg));
// 			}
// 		}
// 	}
// #endif // #if USING_CHEAT_MANAGER
}

bool AOHDPlayerController::ServerCheatAll_Validate(const FString& Msg)
{
	return true;
}

void AOHDPlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);
}

void AOHDPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UOHDAbilitySystemComponent* OHDASC = GetOHDAbilitySystemComponent())
	{
		OHDASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void AOHDPlayerController::OnCameraPenetratingTarget()
{
	bHideViewTargetPawnNextFrame = true;
}

void AOHDPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
// todo primary developer settings
// #if WITH_SERVER_CODE && WITH_EDITOR
// 	if (GIsEditor && (InPawn != nullptr) && (GetPawn() == InPawn))
// 	{
// 		for (const FOHDCheatToRun& CheatRow : GetDefault<UOHDDeveloperSettings>()->CheatsToRun)
// 		{
// 			if (CheatRow.Phase == ECheatExecutionTime::OnPlayerPawnPossession)
// 			{
// 				ConsoleCommand(CheatRow.Cheat, /*bWriteToLog=*/ true);
// 			}
// 		}
// 	}
//#endif

	SetIsAutoRunning(false);
}

void AOHDPlayerController::SetIsAutoRunning(const bool bEnabled)
{
	const bool bIsAutoRunning = GetIsAutoRunning();
	if (bEnabled != bIsAutoRunning)
	{
		if (!bEnabled)
		{
			OnEndAutoRun();
		}
		else
		{
			OnStartAutoRun();
		}
	}
}

bool AOHDPlayerController::GetIsAutoRunning() const
{
	bool bIsAutoRunning = false;
	if (const UOHDAbilitySystemComponent* OHDASC = GetOHDAbilitySystemComponent())
	{
		bIsAutoRunning = OHDASC->GetTagCount(FOHDGameplayTags::Get().Status_AutoRunning) > 0;
	}
	return bIsAutoRunning;
}

void AOHDPlayerController::OnStartAutoRun()
{
	if (UOHDAbilitySystemComponent* OHDASC = GetOHDAbilitySystemComponent())
	{
		OHDASC->SetLooseGameplayTagCount(FOHDGameplayTags::Get().Status_AutoRunning, 1);
		K2_OnStartAutoRun();
	}	
}

void AOHDPlayerController::OnEndAutoRun()
{
	if (UOHDAbilitySystemComponent* OHDASC = GetOHDAbilitySystemComponent())
	{
		OHDASC->SetLooseGameplayTagCount(FOHDGameplayTags::Get().Status_AutoRunning, 0);
		K2_OnEndAutoRun();
	}
}

void AOHDPlayerController::UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId)
{
	if (bForceFeedbackEnabled)
	{
		if (const UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(GetLocalPlayer()))
		{
			const ECommonInputType CurrentInputType = CommonInputSubsystem->GetCurrentInputType();
			if (CurrentInputType == ECommonInputType::Gamepad || CurrentInputType == ECommonInputType::Touch)
			{
				InputInterface->SetForceFeedbackChannelValues(ControllerId, ForceFeedbackValues);
				return;
			}
		}
	}
	
	InputInterface->SetForceFeedbackChannelValues(ControllerId, FForceFeedbackValues());
}

void AOHDPlayerController::UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents)
{
	Super::UpdateHiddenComponents(ViewLocation, OutHiddenComponents);

	if (bHideViewTargetPawnNextFrame)
	{
		AActor* const ViewTargetPawn = PlayerCameraManager ? Cast<AActor>(PlayerCameraManager->GetViewTarget()) : nullptr;
		if (ViewTargetPawn)
		{
			// internal helper func to hide all the components
			auto AddToHiddenComponents = [&OutHiddenComponents](const TInlineComponentArray<UPrimitiveComponent*>& InComponents)
			{
				// add every component and all attached children
				for (UPrimitiveComponent* Comp : InComponents)
				{
					if (Comp->IsRegistered())
					{
						OutHiddenComponents.Add(Comp->ComponentId);

						for (USceneComponent* AttachedChild : Comp->GetAttachChildren())
						{
							static FName NAME_NoParentAutoHide(TEXT("NoParentAutoHide"));
							UPrimitiveComponent* AttachChildPC = Cast<UPrimitiveComponent>(AttachedChild);
							if (AttachChildPC && AttachChildPC->IsRegistered() && !AttachChildPC->ComponentTags.Contains(NAME_NoParentAutoHide))
							{
								OutHiddenComponents.Add(AttachChildPC->ComponentId);
							}
						}
					}
				}
			};

			//TODO Solve with an interface.  Gather hidden components or something.
			//TODO Hiding isn't awesome, sometimes you want the effect of a fade out over a proximity, needs to bubble up to designers.

			// hide pawn's components
			TInlineComponentArray<UPrimitiveComponent*> PawnComponents;
			ViewTargetPawn->GetComponents(PawnComponents);
			AddToHiddenComponents(PawnComponents);

			//// hide weapon too
			//if (ViewTargetPawn->CurrentWeapon)
			//{
			//	TInlineComponentArray<UPrimitiveComponent*> WeaponComponents;
			//	ViewTargetPawn->CurrentWeapon->GetComponents(WeaponComponents);
			//	AddToHiddenComponents(WeaponComponents);
			//}
		}

		// we consumed it, reset for next frame
		bHideViewTargetPawnNextFrame = false;
	}
}
//
// void AOHDPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
// {
// 	UE_LOG(LogOHDTeams, Error, TEXT("You can't set the team ID on a player controller (%s); it's driven by the associated player state"), *GetPathNameSafe(this));
// }

// FGenericTeamId AOHDPlayerController::GetGenericTeamId() const
// {
// 	if (const IOHDTeamAgentInterface* PSWithTeamInterface = Cast<IOHDTeamAgentInterface>(PlayerState))
// 	{
// 		return PSWithTeamInterface->GetGenericTeamId();
// 	}
// 	return FGenericTeamId::NoTeam;
// }
//
// FOnOHDTeamIndexChangedDelegate* AOHDPlayerController::GetOnTeamIndexChangedDelegate()
// {
// 	return &OnTeamChangedDelegate;
// }

void AOHDPlayerController::OnUnPossess()
{
	// Make sure the pawn that is being unpossessed doesn't remain our ASC's avatar actor
	if (APawn* PawnBeingUnpossessed = GetPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
		{
			if (ASC->GetAvatarActor() == PawnBeingUnpossessed)
			{
				ASC->SetAvatarActor(nullptr);
			}
		}
	}

	Super::OnUnPossess();
}

//////////////////////////////////////////////////////////////////////
// AOHDReplayPlayerController

void AOHDReplayPlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);
}

