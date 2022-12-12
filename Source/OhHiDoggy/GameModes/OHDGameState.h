// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameState.h"
#include "OHDGameState.generated.h"

class UOHDExperienceManagerComponent;

UCLASS(Config = Game)
class OHHIDOGGY_API AOHDGameState : public AGameStateBase//public IAbilitySystemInterface, todo add modular inter inheritance
{
	GENERATED_BODY()


public:

	AOHDGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	float GetServerFPS() const { return ServerFPS; }

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor interface

	//~AGameStateBase interface
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	//~End of AGameStateBase interface

	//~IAbilitySystemInterface
	//virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;//todo
	//~End of IAbilitySystemInterface

	// UFUNCTION(BlueprintCallable, Category = "OHD|GameState")//todo
	// UOHDAbilitySystemComponent* GetOHDAbilitySystemComponent() const { return AbilitySystemComponent; }

	// Send a message that all clients will (probably) get
	// (use only for client notifications like eliminations, server join messages, etc... that can handle being lost)
	// UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category = "OHD|GameState")//todo
	// void MulticastMessageToClients(const FOHDVerbMessage Message);

	// Send a message that all clients will be guaranteed to get
	// (use only for client notifications that cannot handle being lost)
	// UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "OHD|GameState")//todo
	// void MulticastReliableMessageToClients(const FOHDVerbMessage Message);

private:
	UPROPERTY()
	UOHDExperienceManagerComponent* ExperienceManagerComponent;

	// The ability system component subobject for game-wide things (primarily gameplay cues)
	//UPROPERTY(VisibleAnywhere, Category = "OHD|GameState")
	//UOHDAbilitySystemComponent* AbilitySystemComponent;//todo ability impl


protected:

	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(Replicated)
	float ServerFPS;
};
