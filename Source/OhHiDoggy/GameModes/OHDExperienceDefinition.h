// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OhHiDoggy/Data/OhHiDoggyPawnData.h"
#include "UObject/Object.h"
#include "OHDExperienceDefinition.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Const)
class OHHIDOGGY_API UOHDExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

	
public:
	UOHDExperienceDefinition();

	//~UObject interface
	#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
#endif
	//~End of UObject interface

	//~UPrimaryDataAsset interface
	#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif
	//~End of UPrimaryDataAsset interface

	public:
	// List of Game Feature Plugins this experience wants to have active
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<FString> GameFeaturesToEnable;

	/** The default pawn class to spawn for players */
	//@TODO: Make soft?
	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TObjectPtr<const UOhHiDoggyPawnData> DefaultPawnData;

	// List of actions to perform as this experience is loaded/activated/deactivated/unloaded
	// UPROPERTY(EditDefaultsOnly, Instanced, Category="Actions")
	// TArray<UGameFeatureAction*> Actions;

	// List of additional action sets to compose into this experience
	// UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	// TArray<TObjectPtr<UOHDExperienceActionSet>> ActionSets;
};
