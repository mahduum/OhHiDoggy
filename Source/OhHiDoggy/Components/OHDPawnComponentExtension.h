﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OHDPawnComponent.h"
#include "OhHiDoggy/Data/OHDPawnData.h"
#include "OHDPawnComponentExtension.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOHDDynamicMulticastDelegate);

class UAbilitySystemComponent;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OHHIDOGGY_API UOHDPawnComponentExtension : public UOHDPawnComponent
{
	GENERATED_BODY()
public:

	UOHDPawnComponentExtension(const FObjectInitializer& ObjectInitializer);

	// Returns the pawn extension component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "OhHiDoggy|Pawn")
	static UOHDPawnComponentExtension* FindPawnExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UOHDPawnComponentExtension>() : nullptr); }

	template <class T>
	const T* GetPawnData() const { return Cast<UOHDPawnData>(PawnData); }

	void SetPawnData(const UOHDPawnData* InPawnData);//todo I already done it somewhere with data?

	UFUNCTION(BlueprintPure, Category = "OhHiDoggy|Pawn")
	UOHDAbilitySystemComponent* GetOHDAbilitySystemComponent() const { return AbilitySystemComponent; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// // Should be called by the owning pawn to become the avatar of the ability system.
	void InitializeAbilitySystem(UOHDAbilitySystemComponent* InASC, AActor* InOwnerActor);//todo primary where its called
	
	// // Should be called by the owning pawn to remove itself as the avatar of the ability system.
	void UninitializeAbilitySystem();
	//
	// Should be called by the owning pawn when the pawn's controller changes.
	void HandleControllerChanged();
	//
	// // Should be called by the owning pawn when the player state has been replicated.
	// void HandlePlayerStateReplicated();//todo and where it's called

	// Should be called by the owning pawn when the input component is setup.
	void SetupPlayerInputComponent();

	// Call this anytime the pawn needs to check if it's ready to be initialized (pawn data assigned, possessed, etc..). 
	bool CheckPawnReadyToInitialize();

	// Returns true if the pawn is ready to be initialized.
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "OhHiDoggy|Pawn", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsPawnReadyToInitialize() const { return bPawnReadyToInitialize; }
	
	// Register with the OnPawnReadyToInitialize delegate and broadcast if condition is already met.
	// Anything that depends on this pawn will delegate its own initialization to when the PawnExt and all its dependencies are ready and
	// any requirements of other components are met (any component with requirements has it specified by an interface implementation)
	void OnPawnReadyToInitialize_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);
	
	// // Register with the OnAbilitySystemInitialized delegate and broadcast if condition is already met.
	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);//todo primary and where it's called
	//
	// // Register with the OnAbilitySystemUninitialized delegate.
	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);//todo primary and where it's called

protected:

	virtual void OnRegister() override;

	UFUNCTION()
	void OnRep_PawnData();

	// Delegate fired when pawn has everything needed for initialization.
	FSimpleMulticastDelegate OnPawnReadyToInitialize;

	UPROPERTY(BlueprintAssignable, Meta = (DisplayName = "On Pawn Ready To Initialize"))
	FOHDDynamicMulticastDelegate BP_OnPawnReadyToInitialize;

	// Delegate fired when our pawn becomes the ability system's avatar actor
	FSimpleMulticastDelegate OnAbilitySystemInitialized;

	// Delegate fired when our pawn is removed as the ability system's avatar actor
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;

protected:

	// Pawn data used to create the pawn.  Specified from a spawn function or on a placed instance.
	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_PawnData, Category = "OhHiDoggy|Pawn")
	const UOHDPawnData* PawnData;

	// Pointer to the ability system component that is cached for convenience.
	UPROPERTY()
	UOHDAbilitySystemComponent* AbilitySystemComponent;

	// True when the pawn has everything needed for initialization.
	int32 bPawnReadyToInitialize : 1;
};
