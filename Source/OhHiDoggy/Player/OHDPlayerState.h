// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerState.h"
#include "OhHiDoggy/Data/OhHiDoggyPawnData.h"
#include "OHDPlayerState.generated.h"


class UOHDExperienceDefinition;
/** Defines the types of client connected */
UENUM()
enum class EOHDPlayerConnectionType : uint8
{
	// An active player
	Player = 0,

	// Spectator connected to a running game
	LiveSpectator,

	// Spectating a demo recording offline
	ReplaySpectator,

	// A deactivated player (disconnected)
	InactivePlayer
};

UCLASS(Config = Game)
class OHHIDOGGY_API AOHDPlayerState : public APlayerState//todo change it to use modular player state from the module ModularGameplayActors
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AOHDPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// UFUNCTION(BlueprintCallable, Category = "OHD|PlayerState")
	// AOHDPlayerController* GetOHDPlayerController() const;

	// UFUNCTION(BlueprintCallable, Category = "OHD|PlayerState")
	// UOHDAbilitySystemComponent* GetOHDAbilitySystemComponent() const { return AbilitySystemComponent; }
	// virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const UOhHiDoggyPawnData* InPawnData);

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	//~End of AActor interface

	//~APlayerState interface
	virtual void Reset() override;
	virtual void ClientInitialize(AController* C) override;
	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void OnDeactivated() override;
	virtual void OnReactivated() override;
	//~End of APlayerState interface

	static const FName NAME_OHDAbilityReady;

	void SetPlayerConnectionType(EOHDPlayerConnectionType NewType);
	EOHDPlayerConnectionType GetPlayerConnectionType() const { return MyPlayerConnectionType; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void OnExperienceLoaded(const UOHDExperienceDefinition* CurrentExperience);

protected:
	UFUNCTION()
	void OnRep_PawnData();

protected:

	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	const UOhHiDoggyPawnData* PawnData;

private:

	// The ability system component sub-object used by player characters.
	// UPROPERTY(VisibleAnywhere, Category = "OHD|PlayerState")
	// UOHDAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(Replicated)
	EOHDPlayerConnectionType MyPlayerConnectionType;

};
