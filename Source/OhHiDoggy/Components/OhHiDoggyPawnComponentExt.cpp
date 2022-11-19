// Fill out your copyright notice in the Description page of Project Settings.


#include "OhHiDoggyPawnComponentExt.h"
#include "Net/UnrealNetwork.h"


UOhHiDoggyPawnComponentExt::UOhHiDoggyPawnComponentExt(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	//PawnData = nullptr;//todo also add pawn data to lifetime replicated props
	AbilitySystemComponent = nullptr;
	bPawnReadyToInitialize = false;
}

void UOhHiDoggyPawnComponentExt::OnPawnReadyToInitialize_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnPawnReadyToInitialize.IsBoundToObject(Delegate.GetUObject()))
	{
		OnPawnReadyToInitialize.Add(Delegate);
	}

	if (bPawnReadyToInitialize)
	{
		Delegate.Execute();
	}
}

void UOhHiDoggyPawnComponentExt::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("LyraPawnExtensionComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> PawnExtensionComponents;
	Pawn->GetComponents(UOhHiDoggyPawnComponentExt::StaticClass(), PawnExtensionComponents);
	ensureAlwaysMsgf((PawnExtensionComponents.Num() == 1), TEXT("Only one LyraPawnExtensionComponent should exist on [%s]."), *GetNameSafe(GetOwner()));
}

void UOhHiDoggyPawnComponentExt::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UOhHiDoggyPawnComponentExt::SetupPlayerInputComponent()
{
	CheckPawnReadyToInitialize();
}

void UOhHiDoggyPawnComponentExt::OnRep_PawnData()
{
	CheckPawnReadyToInitialize();
}

bool UOhHiDoggyPawnComponentExt::CheckPawnReadyToInitialize()//todo what should be calling this? It is called with various setters like SetPawnData or SetupInputComponent
{
	if (bPawnReadyToInitialize)
	{
		return true;
	}

	// Pawn data is required.
	// if (!PawnData)
	// {
	// 	return false;
	// }

	APawn* Pawn = GetPawnChecked<APawn>();

	const bool bHasAuthority = Pawn->HasAuthority();
	const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

	if (bHasAuthority || bIsLocallyControlled)
	{
		// Check for being possessed by a controller.
		if (!GetController<AController>())
		{
			return false;
		}
	}

	// Allow pawn components to have requirements.//todo debug Lyra to find what exactly are those components
	//todo: REMEMBER query for interfaces using UClass, but use them via their IInterface object
	TArray<UActorComponent*> InteractableComponents = Pawn->GetComponentsByInterface(UOhHiDoggyReadyInterface::StaticClass());
	for (UActorComponent* InteractableComponent : InteractableComponents)
	{
		const IOhHiDoggyReadyInterface* Ready = CastChecked<IOhHiDoggyReadyInterface>(InteractableComponent);
		if (!Ready->IsPawnComponentReadyToInitialize())
		{
			return false;
		}
	}

	// Pawn is ready to initialize.
	bPawnReadyToInitialize = true;
	OnPawnReadyToInitialize.Broadcast();
	//BP_OnPawnReadyToInitialize.Broadcast();

	return true;
}


void UOhHiDoggyPawnComponentExt::SetPawnData(const UOhHiDoggyPawnData* InPawnData)//todo make and understand what should be pawn data, make it and set it
{
	check(InPawnData);

	bPawnReadyToInitialize = false;

	APawn* Pawn = GetPawnChecked<APawn>();

	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		UE_LOG(LogCore, Error, TEXT("Trying to set PawnData [%s] on pawn [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(Pawn), *GetNameSafe(PawnData));
		return;
	}

	PawnData = InPawnData;

	Pawn->ForceNetUpdate();

	CheckPawnReadyToInitialize();
}
