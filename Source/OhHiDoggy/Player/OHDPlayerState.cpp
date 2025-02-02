﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "OHDPlayerState.h"

#include "OHDPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "OhHiDoggy/AbilitySystem/OHDAbilitySystemComponent.h"
#include "OhHiDoggy/Components/OHDPawnComponentExtension.h"
#include "OhHiDoggy/GameModes/OHDExperienceManagerComponent.h"
#include "OhHiDoggy/GameModes/OHDGameMode.h"
#include "OhHiDoggy/OHDLogChannels.h"
#include "OhHiDoggy/AbilitySystem/OHDAbilitySet.h"


AOHDPlayerState::AOHDPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MyPlayerConnectionType(EOHDPlayerConnectionType::Player)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UOHDAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	UE_LOG(LogOHD, Display, TEXT("Ability system created: %s"), *AbilitySystemComponent->GetFullName());


	//CreateDefaultSubobject<UOHDHealthSet>(TEXT("HealthSet"));//todo primary health set
	//CreateDefaultSubobject<UOHDCombatSet>(TEXT("CombatSet"));

	// AbilitySystemComponent needs to be updated at a high frequency.
	NetUpdateFrequency = 100.0f;
}

void AOHDPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AOHDPlayerState::Reset()
{
	Super::Reset();
}

void AOHDPlayerState::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);

	if (UOHDPawnComponentExtension* PawnExtComp = UOHDPawnComponentExtension::FindPawnExtensionComponent(GetPawn()))
	{
		PawnExtComp->CheckPawnReadyToInitialize();
	}
}

void AOHDPlayerState::CopyProperties(APlayerState* PlayerState)
{
	//Super::CopyProperties(PlayerState);

	//@TODO: Copy stats
}

void AOHDPlayerState::OnDeactivated()
{
	bool bDestroyDeactivatedPlayerState = false;

	switch (GetPlayerConnectionType())
	{
		case EOHDPlayerConnectionType::Player:
		case EOHDPlayerConnectionType::InactivePlayer:
			//@TODO: Ask the experience if we should destroy disconnecting players immediately or leave them around
			// (e.g., for long running servers where they might build up if lots of players cycle through)
			bDestroyDeactivatedPlayerState = true;
			break;
		default:
			bDestroyDeactivatedPlayerState = true;
			break;
	}
	
	SetPlayerConnectionType(EOHDPlayerConnectionType::InactivePlayer);

	if (bDestroyDeactivatedPlayerState)
	{
		Destroy();
	}
}

void AOHDPlayerState::OnReactivated()
{
	if (GetPlayerConnectionType() == EOHDPlayerConnectionType::InactivePlayer)
	{
		SetPlayerConnectionType(EOHDPlayerConnectionType::Player);
	}
}

void AOHDPlayerState::OnExperienceLoaded(const UOHDExperienceDefinition* /*CurrentExperience*/)
{
	//todo priamary why is this commented? not used?
	if (const AOHDGameMode* OHDGameMode = GetWorld()->GetAuthGameMode<AOHDGameMode>())
	{
		if (const UOHDPawnData* NewPawnData = OHDGameMode->GetPawnDataForController(GetOwningController()))
		{
			SetPawnData(NewPawnData);
		}
		else
		{
			UE_LOG(LogCore, Error, TEXT("AOHDPlayerState::OnExperienceLoaded(): Unable to find PawnData to initialize player state [%s]!"), *GetNameSafe(this));
		}
	}
}

void AOHDPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyPlayerConnectionType, SharedParams)
}

AOHDPlayerController* AOHDPlayerState::GetOHDPlayerController() const
{
	return Cast<AOHDPlayerController>(GetOwner());
}

UAbilitySystemComponent* AOHDPlayerState::GetAbilitySystemComponent() const
{
	return GetOHDAbilitySystemComponent();
}

void AOHDPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// todo
	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
	
	if (GetNetMode() != NM_Client)
	{
		AGameStateBase* GameState = GetWorld()->GetGameState();
		check(GameState);
		UOHDExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UOHDExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnOHDExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}
}

void AOHDPlayerState::SetPawnData(const UOHDPawnData* InPawnData)
{
	check(InPawnData);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		UE_LOG(LogCore, Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;

	// todo primary ability set 
	for (const UOHDAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}
	
	// UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_OHDAbilityReady);
	
	ForceNetUpdate();
}

void AOHDPlayerState::OnRep_PawnData()
{
}

void AOHDPlayerState::SetPlayerConnectionType(EOHDPlayerConnectionType NewType)
{
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MyPlayerConnectionType, this);
	MyPlayerConnectionType = NewType;
}

// Called when the game starts or when spawned
void AOHDPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOHDPlayerState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

