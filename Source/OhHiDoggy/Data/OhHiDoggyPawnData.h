// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OhHiDoggyPawnData.generated.h"

class UOHDAbilityTagRelationshipMapping;
class UDoggyInputConfig;
/*
 * UOhHiDoggyPawnData
 *
 * Non-mutable data asset that contains properties used to define a pawn.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "OhHiDoggy Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class OHHIDOGGY_API UOhHiDoggyPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

	
public:

	UOhHiDoggyPawnData(const FObjectInitializer& ObjectInitializer);

public:

	// Class to instantiate for this pawn (should usually derive from ALyraPawn or ACanineCharacter). Is retrieved by game mode through controller (where the owner - pawn data -
	// is kept/loaded to from player state or experience: todo primary make custom player state and experience definition in world settings
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OhHiDoggy|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system. //todo primary add class sets
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OhHiDoggy|Abilities")//todo make from primary data asset
	// TArray<ULyraAbilitySet*> AbilitySets;//todo abilities with data: ability class, ability level and ability input tag, gameplay effects and owner attribute sets

	// What mapping of ability tags to use for actions taking by this pawn//todo make from udataasset (tags to cancel, tags required, what other abilities is it blocking etc.)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OhHiDoggy|Abilities")
	UOHDAbilityTagRelationshipMapping* TagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OhHiDoggy|Input")
	UDoggyInputConfig* InputConfig;

	// // Default camera mode used by player controlled pawns.
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OhHiDoggy|Camera")
	// TSubclassOf<ULyraCameraMode> DefaultCameraMode;//TODO primary add camera modes, modes are in stack variable inside custom camera component
};
