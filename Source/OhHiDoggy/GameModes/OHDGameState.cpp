// Fill out your copyright notice in the Description page of Project Settings.


#include "OHDGameState.h"
#include "AbilitySystemComponent.h"
#include "OHDExperienceManagerComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
//#include "GameFramework/GameplayMessageSubsystem.h"


extern ENGINE_API float GAverageFPS;


AOHDGameState::AOHDGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	//AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UOHDAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));//todo
	//AbilitySystemComponent->SetIsReplicated(true);
	//AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	ExperienceManagerComponent = CreateDefaultSubobject<UOHDExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
	UE_LOG(LogCore, Display, TEXT("Game state class contructor called, subobject created: %s"), *ExperienceManagerComponent->GetName());

	ServerFPS = 0.0f;
}

void AOHDGameState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AOHDGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// check(AbilitySystemComponent);
	// AbilitySystemComponent->InitAbilityActorInfo(/*Owner=*/ this, /*Avatar=*/ this);//todo ability system
}

// UAbilitySystemComponent* AOHDGameState::GetAbilitySystemComponent() const
// {
// 	return AbilitySystemComponent;
// }

void AOHDGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AOHDGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
}

void AOHDGameState::RemovePlayerState(APlayerState* PlayerState)
{
	//@TODO: This isn't getting called right now (only the 'rich' AGameMode uses it, not AGameModeBase)
	// Need to at least comment the engine code, and possibly move things around
	Super::RemovePlayerState(PlayerState);
}

void AOHDGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ServerFPS);
}

void AOHDGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetLocalRole() == ROLE_Authority)
	{
		ServerFPS = GAverageFPS;
	}
}
//
// void AOHDGameState::MulticastMessageToClients_Implementation(const FOHDVerbMessage Message)
// {
// 	if (GetNetMode() == NM_Client)
// 	{
// 		UGameplayMessageSubsystem::Get(this).BroadcastMessage(Message.Verb, Message);
// 	}
// }
//
// void AOHDGameState::MulticastReliableMessageToClients_Implementation(const FOHDVerbMessage Message)
// {
// 	MulticastMessageToClients_Implementation(Message);
// }

