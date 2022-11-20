// Fill out your copyright notice in the Description page of Project Settings.


#include "OHDGameMode.h"
#include "OHDExperienceDefinition.h"
//#include "OHDWorldSettings.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/GameState.h"
#include "GameFramework/HUD.h"
#include "OhHiDoggy/CanineCharacter/CanineCharacter.h"
#include "OhHiDoggy/Player/OHDPlayerState.h"

AOHDGameMode::AOHDGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = AGameState::StaticClass();//todo gamemode custom
	GameSessionClass = AGameSession::StaticClass();//todo gamemode custom
	PlayerControllerClass = APlayerController::StaticClass();//todo gamemode custom
	//ReplaySpectatorPlayerControllerClass = AOHDReplayPlayerController::StaticClass();//todo gamemode custom
	PlayerStateClass = AOHDPlayerState::StaticClass();
	DefaultPawnClass = ACanineCharacter::StaticClass();
	HUDClass = AHUD::StaticClass();//todo gamemode custom
}

const UOHDPawnData* AOHDGameMode::GetPawnDataForController(const AController* InController) const
{
	// See if pawn data is already set on the player state
	if (InController != nullptr)
	{
		if (const AOHDPlayerState* OHDPS = InController->GetPlayerState<AOHDPlayerState>())
		{
			if (const UOHDPawnData* PawnData = OHDPS->GetPawnData<UOHDPawnData>())
			{
				return PawnData;
			}
		}
	}

	// If not, fall back to the the default for the current experience todo experience
	// check(GameState);
	// UOHDExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UOHDExperienceManagerComponent>();
	// check(ExperienceComponent);
	//
	// if (ExperienceComponent->IsExperienceLoaded())
	// {
	// 	const UOHDExperienceDefinition* Experience = ExperienceComponent->GetCurrentExperienceChecked();
	// 	if (Experience->DefaultPawnData != nullptr)
	// 	{
	// 		return Experience->DefaultPawnData;
	// 	}
	//
	// 	// Experience is loaded and there's still no pawn data, fall back to the default for now
	// 	return UOHDAssetManager::Get().GetDefaultPawnData();
	// }

	// Experience not loaded yet, so there is no pawn data to be had
	return nullptr;
}

void AOHDGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	//@TODO: Eventually only do this for PIE/auto
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void AOHDGameMode::HandleMatchAssignmentIfNotExpectingOne()
{
	// FPrimaryAssetId ExperienceId;
	// FString ExperienceIdSource;
	//
	// // Precedence order (highest wins)
	// //  - Matchmaking assignment (if present)
	// //  - URL Options override
	// //  - Developer Settings (PIE only)
	// //  - Command Line override
	// //  - World Settings
	// //  - Default experience
	//
	// UWorld* World = GetWorld();
	//
	// if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	// {
	// 	const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
	// 	ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UOHDExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));
	// 	ExperienceIdSource = TEXT("OptionsString");
	// }
	//
	// if (!ExperienceId.IsValid() && World->IsPlayInEditor())
	// {
	// 	ExperienceId = GetDefault<UOHDDeveloperSettings>()->ExperienceOverride;
	// 	ExperienceIdSource = TEXT("DeveloperSettings");
	// }
	//
	// // see if the command line wants to set the experience
	// if (!ExperienceId.IsValid())
	// {
	// 	FString ExperienceFromCommandLine;
	// 	if (FParse::Value(FCommandLine::Get(), TEXT("Experience="), ExperienceFromCommandLine))
	// 	{
	// 		ExperienceId = FPrimaryAssetId::ParseTypeAndName(ExperienceFromCommandLine);
	// 		ExperienceIdSource = TEXT("CommandLine");
	// 	}
	// }
	//
	// // see if the world settings has a default experience
	// if (!ExperienceId.IsValid())
	// {
	// 	if (AOHDWorldSettings* TypedWorldSettings = Cast<AOHDWorldSettings>(GetWorldSettings()))
	// 	{
	// 		ExperienceId = TypedWorldSettings->GetDefaultGameplayExperience();
	// 		ExperienceIdSource = TEXT("WorldSettings");
	// 	}
	// }
	//
	// UOHDAssetManager& AssetManager = UOHDAssetManager::Get();
	// FAssetData Dummy;
	// if (ExperienceId.IsValid() && !AssetManager.GetPrimaryAssetData(ExperienceId, /*out*/ Dummy))
	// {
	// 	UE_LOG(LogCore, Error, TEXT("EXPERIENCE: Wanted to use %s but couldn't find it, falling back to the default)"), *ExperienceId.ToString());
	// 	ExperienceId = FPrimaryAssetId();
	// }
	//
	// // Final fallback to the default experience
	// if (!ExperienceId.IsValid())
	// {
	// 	//@TODO: Pull this from a config setting or something
	// 	ExperienceId = FPrimaryAssetId(FPrimaryAssetType("OHDExperienceDefinition"), FName("B_OHDDefaultExperience"));
	// 	ExperienceIdSource = TEXT("Default");
	// }
	//
	// OnMatchAssignmentGiven(ExperienceId, ExperienceIdSource);
}

void AOHDGameMode::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource)
{
// #if WITH_SERVER_CODE
// 	if (ExperienceId.IsValid())
// 	{
// 		UE_LOG(LogCore, Log, TEXT("Identified experience %s (Source: %s)"), *ExperienceId.ToString(), *ExperienceIdSource);
//
// 		UOHDExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UOHDExperienceManagerComponent>();
// 		check(ExperienceComponent);
// 		ExperienceComponent->ServerSetCurrentExperience(ExperienceId);
// 	}
// 	else
// 	{
// 		UE_LOG(LogCore, Error, TEXT("Failed to identify experience, loading screen will stay up forever"));
// 	}
// #endif
}

void AOHDGameMode::OnExperienceLoaded(const UOHDExperienceDefinition* CurrentExperience)
{
	// Spawn any players that are already attached
	//@TODO: Here we're handling only *player* controllers, but in GetDefaultPawnClassForController_Implementation we skipped all controllers
	// GetDefaultPawnClassForController_Implementation might only be getting called for players anyways
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);
		if ((PC != nullptr) && (PC->GetPawn() == nullptr))
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
}

bool AOHDGameMode::IsExperienceLoaded() const
{
	// check(GameState);
	// UOHDExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UOHDExperienceManagerComponent>();
	// check(ExperienceComponent);
	//
	// return ExperienceComponent->IsExperienceLoaded();
	return false; //todo experience
}

UClass* AOHDGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UOHDPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* AOHDGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// Never save the default player pawns into a map.
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			if (UOhHiDoggyPawnComponentExt* PawnExtComp = UOhHiDoggyPawnComponentExt::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const UOHDPawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);
				}
				else
				{
					UE_LOG(LogCore, Error, TEXT("Game mode was unable to set PawnData on the spawned pawn [%s]."), *GetNameSafe(SpawnedPawn));
				}
			}

			SpawnedPawn->FinishSpawning(SpawnTransform);

			return SpawnedPawn;
		}
		else
		{
			UE_LOG(LogCore, Error, TEXT("Game mode was unable to spawn Pawn of class [%s] at [%s]."), *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
		}
	}
	else
	{
		UE_LOG(LogCore, Error, TEXT("Game mode was unable to spawn Pawn due to NULL pawn class."));
	}

	return nullptr;
}

bool AOHDGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	// We never want to use the start spot, always use the spawn management component.
	return false;
}

void AOHDGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Delay starting new players until the experience has been loaded
	// (players who log in prior to that will be started by OnExperienceLoaded)
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

AActor* AOHDGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// todo player start
	// if (UOHDPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<UOHDPlayerSpawningManagerComponent>())
	// {
	// 	return PlayerSpawningComponent->ChoosePlayerStart(Player);
	// }
	
	return Super::ChoosePlayerStart_Implementation(Player);
}

void AOHDGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	// todo player start

	// if (UOHDPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<UOHDPlayerSpawningManagerComponent>())
	// {
	// 	PlayerSpawningComponent->FinishRestartPlayer(NewPlayer, StartRotation);
	// }

	Super::FinishRestartPlayer(NewPlayer, StartRotation);
}

bool AOHDGameMode::PlayerCanRestart_Implementation(APlayerController* Player)
{
	return ControllerCanRestart(Player);
}

bool AOHDGameMode::ControllerCanRestart(AController* Controller)
{
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{	
		if (!Super::PlayerCanRestart_Implementation(PC))
		{
			return false;
		}
	}
	else
	{
		// Bot version of Super::PlayerCanRestart_Implementation
		if ((Controller == nullptr) || Controller->IsPendingKillPending())
		{
			return false;
		}
	}

	// todo player start

	// if (UOHDPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<UOHDPlayerSpawningManagerComponent>())
	// {
	// 	return PlayerSpawningComponent->ControllerCanRestart(Controller);
	// }

	return true;
}

void AOHDGameMode::InitGameState()
{
	Super::InitGameState();

	// todo experience
	// Listen for the experience load to complete	
	// UOHDExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UOHDExperienceManagerComponent>();
	// check(ExperienceComponent);
	// ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnOHDExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void AOHDGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	// todo experience
	//OnGameModeCombinedPostLoginDelegate.Broadcast(this, NewPlayer);
}

void AOHDGameMode::RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset)
{
	if (bForceReset && (Controller != nullptr))
	{
		Controller->Reset();
	}

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		GetWorldTimerManager().SetTimerForNextTick(PC, &APlayerController::ServerRestartPlayer_Implementation);
	}
	// else if (AOHDPlayerBotController* BotController = Cast<AOHDPlayerBotController>(Controller))//todo game mode bot
	// {
	// 	GetWorldTimerManager().SetTimerForNextTick(BotController, &AOHDPlayerBotController::ServerRestartController);
	// }
}

bool AOHDGameMode::UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage)
{
	// Do nothing, we'll wait until PostLogin when we try to spawn the player for real.
	// Doing anything right now is no good, systems like team assignment haven't even occurred yet.
	return true;
}

void AOHDGameMode::FailedToRestartPlayer(AController* NewPlayer)
{
	Super::FailedToRestartPlayer(NewPlayer);

	// If we tried to spawn a pawn and it failed, lets try again *note* check if there's actually a pawn class
	// before we try this forever.
	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APlayerController* NewPC = Cast<APlayerController>(NewPlayer))
		{
			// If it's a player don't loop forever, maybe something changed and they can no longer restart if so stop trying.
			if (PlayerCanRestart(NewPC))
			{
				RequestPlayerRestartNextFrame(NewPlayer, false);			
			}
			else
			{
				UE_LOG(LogCore, Verbose, TEXT("FailedToRestartPlayer(%s) and PlayerCanRestart returned false, so we're not going to try again."), *GetPathNameSafe(NewPlayer));
			}
		}
		else
		{
			RequestPlayerRestartNextFrame(NewPlayer, false);
		}
	}
	else
	{
		UE_LOG(LogCore, Verbose, TEXT("FailedToRestartPlayer(%s) but there's no pawn class so giving up."), *GetPathNameSafe(NewPlayer));
	}
}

