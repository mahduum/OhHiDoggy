// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "EnhancedInputSubsystems.h"
#include "Components/PawnComponent.h"
#include "OhHiDoggy/Components/OhHiDoggyPawnComponent.h"
#include "OhHiDoggy/Input/MappableConfigPair.h"
#include "DoggyComponent.generated.h"

struct FGameplayTag;
struct FGameplayAbilitySpecHandle;
struct FInputActionValue;
/**
 * 
 */
UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class UDoggyComponent : public UOhHiDoggyPawnComponent//Inspired by ULyraHeroComponent
{
	GENERATED_BODY()
	public:

	//UDoggyComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the hero component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "|Hero")
	static UDoggyComponent* FindHeroComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UDoggyComponent>() : nullptr); }

	//void SetAbilityCameraMode(TSubclassOf<UCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle);
	//void ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle);

	//void AddAdditionalInputConfig(const UDoggyInputConfigBad* InputConfig);
	//void RemoveAdditionalInputConfig(const UDoggyInputConfigBad* InputConfig);

	/** True if this has completed OnPawnReadyToInitialize so is prepared for late-added features */
	//bool HasPawnInitialized() const;

	/** True if this player has sent the BindInputsNow event and is prepared for bindings */
	//bool IsReadyToBindInputs() const;

	static const FName NAME_BindInputsNow;

protected:

	/* Called when component added to object in runtime or in edit mode. */
	virtual void OnRegister() override;

	virtual bool IsPawnComponentReadyToInitialize() const override;//TODO when base class is added add override
	void OnPawnReadyToInitialize();//todo here initialize input

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	// void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	// void Input_AbilityInputTagReleased(FGameplayTag InputTag);
	//
	void Input_Move(const FInputActionValue& InputActionValue) const;
	// void Input_LookMouse(const FInputActionValue& InputActionValue);
	// void Input_LookStick(const FInputActionValue& InputActionValue);
	// void Input_Crouch(const FInputActionValue& InputActionValue);
	// void Input_AutoRun(const FInputActionValue& InputActionValue);

	//TSubclassOf<UCameraMode> DetermineCameraMode() const;
	
	// void OnInputConfigActivated(const FLoadedMappableConfigPair& ConfigPair);
	// void OnInputConfigDeactivated(const FLoadedMappableConfigPair& ConfigPair);

protected:

	/**
	 * Input Configs that should be added to this player when initalizing the input.
	 * 
	 * NOTE: You should only add to this if you do not have a game feature plugin accessible to you.
	 * If you do, then use the GameFeatureAction_AddInputConfig instead. 
	 */
	UPROPERTY(EditAnywhere)
	TArray<FMappableConfigPair> DefaultInputConfigs;
	
	// Camera mode set by an ability.
	//TSubclassOf<UCameraMode> AbilityCameraMode;

	// Spec handle for the last ability to set a camera mode.
	FGameplayAbilitySpecHandle AbilityCameraModeOwningSpecHandle;

	// True when the pawn has fully finished initialization
	bool bPawnHasInitialized;

	// True when player input bindings have been applyed, will never be true for non-players
	bool bReadyToBindInputs;
};
