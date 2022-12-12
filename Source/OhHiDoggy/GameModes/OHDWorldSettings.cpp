// Fill out your copyright notice in the Description page of Project Settings.


#include "OHDWorldSettings.h"

#include "EngineUtils.h"
#include "Engine/AssetManager.h"
#include "GameFramework/PlayerStart.h"
#include "Misc/UObjectToken.h"
#include "OhHiDoggy/System/OHDAssetManager.h"


// Sets default values
AOHDWorldSettings::AOHDWorldSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}


FPrimaryAssetId AOHDWorldSettings::GetDefaultGameplayExperience() const
{
	FPrimaryAssetId Result;
	if (!DefaultGameplayExperience.IsNull())
	{
		Result = UOHDAssetManager::Get().GetPrimaryAssetIdForPath(DefaultGameplayExperience.ToSoftObjectPath());//todo this doesn't work???

		if (!Result.IsValid())
		{
			UE_LOG(LogCore, Error, TEXT("%s.DefaultGameplayExperience is %s but that failed to resolve into an asset ID (you might need to add a path to the Asset Rules in your game feature plugin or project settings"),
				*GetPathNameSafe(this), *DefaultGameplayExperience.ToString());
		}
	}
	return Result;
}

#if WITH_EDITOR
void AOHDWorldSettings::CheckForErrors()
{
	Super::CheckForErrors();

	FMessageLog MapCheck("MapCheck");

	for (TActorIterator<APlayerStart> PlayerStartIt(GetWorld()); PlayerStartIt; ++PlayerStartIt)
	{
		APlayerStart* PlayerStart = *PlayerStartIt;
		if (IsValid(PlayerStart) && PlayerStart->GetClass() == APlayerStart::StaticClass())
		{
			MapCheck.Warning()
				->AddToken(FUObjectToken::Create(PlayerStart))
				->AddToken(FTextToken::Create(FText::FromString("is a normal APlayerStart, replace with AOHDPlayerStart.")));
		}
	}

	//@TODO: Make sure the soft object path is something that can actually be turned into a primary asset ID (e.g., is not pointing to an experience in an unscanned directory)
}
#endif

