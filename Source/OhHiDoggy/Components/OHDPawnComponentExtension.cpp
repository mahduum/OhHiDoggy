// Fill out your copyright notice in the Description page of Project Settings.


#include "OHDPawnComponentExtension.h"
#include "Net/UnrealNetwork.h"
#include "../AbilitySystem/OHDAbilitySystemComponent.h"
#include "../OHDLogChannels.h"


UOHDPawnComponentExtension::UOHDPawnComponentExtension(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	PawnData = nullptr;//todo also add pawn data to lifetime replicated props
	AbilitySystemComponent = nullptr;
	bPawnReadyToInitialize = false;
}

void UOHDPawnComponentExtension::OnPawnReadyToInitialize_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
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

void UOHDPawnComponentExtension::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("OHDPawnExtensionComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> PawnExtensionComponents;
	Pawn->GetComponents(UOHDPawnComponentExtension::StaticClass(), PawnExtensionComponents);
	ensureAlwaysMsgf((PawnExtensionComponents.Num() == 1), TEXT("Only one OHDPawnExtensionComponent should exist on [%s]."), *GetNameSafe(GetOwner()));
}

void UOHDPawnComponentExtension::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UOHDPawnComponentExtension, PawnData);

}


void UOHDPawnComponentExtension::HandleControllerChanged()
{
	if (AbilitySystemComponent && (AbilitySystemComponent->GetAvatarActor() == GetPawnChecked<APawn>()))
	{
		ensure(AbilitySystemComponent->AbilityActorInfo->OwnerActor == AbilitySystemComponent->GetOwnerActor());
		if (AbilitySystemComponent->GetOwnerActor() == nullptr)
		{
			UninitializeAbilitySystem();
		}
		else
		{
			AbilitySystemComponent->RefreshAbilityActorInfo();
		}
	}

	CheckPawnReadyToInitialize();
}

void UOHDPawnComponentExtension::SetupPlayerInputComponent()
{
	CheckPawnReadyToInitialize();
}

void UOHDPawnComponentExtension::OnRep_PawnData()
{
	CheckPawnReadyToInitialize();
}

bool UOHDPawnComponentExtension::CheckPawnReadyToInitialize()//todo what should be calling this? It is called with various setters like SetPawnData or SetupInputComponent
{
	if (bPawnReadyToInitialize)
	{
		UE_LOG(LogCore, Display, TEXT("Pawn is ready to initialize."));

		return true;
	}

	//Pawn data is required.
	if (!PawnData)
	{
		UE_LOG(LogCore, Warning, TEXT("Pawn is missing pawn data."));
		
		return false;
	}

	APawn* Pawn = GetPawnChecked<APawn>();

	const bool bHasAuthority = Pawn->HasAuthority();
	const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

	if (bHasAuthority || bIsLocallyControlled)
	{
		// Check for being possessed by a controller.
		if (!GetController<AController>())
		{
			UE_LOG(LogCore, Warning, TEXT("Pawn is missing controller and is locally controlled."));

			if (Pawn)
			{
				const FString Name = Pawn->GetName();
				UE_LOG(LogCore, Display, TEXT("Pawn's owner name is: %s."), *Name);
			}

			return false;
		}

		UE_LOG(LogCore, Warning, TEXT("Pawn's controller found!"));
	}

	// Allow pawn components to have requirements.//todo debug OHD to find what exactly are those components
	//todo: REMEMBER query for interfaces using UClass, but use them via their IInterface object
	TArray<UActorComponent*> InteractableComponents = Pawn->GetComponentsByInterface(UOhHiDoggyReadyInterface::StaticClass());
	for (UActorComponent* InteractableComponent : InteractableComponents)
	{
		const IOhHiDoggyReadyInterface* Ready = CastChecked<IOhHiDoggyReadyInterface>(InteractableComponent);
		if (!Ready->IsPawnComponentReadyToInitialize())
		{
			UE_LOG(LogCore, Warning, TEXT("Pawn's component is not ready to initialze: %s."), *InteractableComponent->GetName());
			return false;
		}
	}

	// Pawn is ready to initialize.
	// Call whatever has already registered with the delegates, and everything else calling it later will execute instantly as PawnExt is will be already initialized.
	bPawnReadyToInitialize = true;
	
	UE_LOG(LogCore, Warning, TEXT("Pawn is set ready to initialize, braodcasting to delegates."));

	OnPawnReadyToInitialize.Broadcast();
	BP_OnPawnReadyToInitialize.Broadcast();

	return true;
}


void UOHDPawnComponentExtension::SetPawnData(const UOHDPawnData* InPawnData)//todo make and understand what should be pawn data, make it and set it
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


void UOHDPawnComponentExtension::InitializeAbilitySystem(UOHDAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	if (InASC == nullptr)
	{
		// The ability system component hasn't changed.
		UE_LOG(LogOHD, Error, TEXT("Ability system component is nullptr"));
	}
	
	checkf(InASC, TEXT("Ability system component check failed."));
	check(InOwnerActor);

	if (AbilitySystemComponent == InASC)
	{
		// The ability system component hasn't changed.
		UE_LOG(LogOHD, Display, TEXT("Ability system component hasn't changed and is: %s"), *AbilitySystemComponent->GetFullName());
		return;
	}

	if (AbilitySystemComponent)
	{
		// Clean up the old ability system component.
		UninitializeAbilitySystem();
	}

	APawn* Pawn = GetPawnChecked<APawn>();
	AActor* ExistingAvatar = InASC->GetAvatarActor();

	UE_LOG(LogOHD, Display, TEXT("Setting up ASC [%s] on pawn [%s] owner [%s], existing [%s] "), *GetNameSafe(InASC), *GetNameSafe(Pawn), *GetNameSafe(InOwnerActor), *GetNameSafe(ExistingAvatar));

	if ((ExistingAvatar != nullptr) && (ExistingAvatar != Pawn))
	{
		UE_LOG(LogOHD, Log, TEXT("Existing avatar (authority=%d)"), ExistingAvatar->HasAuthority() ? 1 : 0);

		// There is already a pawn acting as the ASC's avatar, so we need to kick it out
		// This can happen on clients if they're lagged: their new pawn is spawned + possessed before the dead one is removed
		ensure(!ExistingAvatar->HasAuthority());

		if (UOHDPawnComponentExtension* OtherExtensionComponent = FindPawnExtensionComponent(ExistingAvatar))
		{
			OtherExtensionComponent->UninitializeAbilitySystem();
		}
	}

	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);

	if (ensure(PawnData))
	{
		InASC->SetTagRelationshipMapping(PawnData->TagRelationshipMapping);
	}

	OnAbilitySystemInitialized.Broadcast();
}

void UOHDPawnComponentExtension::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		AbilitySystemComponent->CancelAbilities(nullptr, nullptr);
		AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();

		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			// If the ASC doesn't have a valid owner, we need to clear *all* actor info, not just the avatar pairing
			AbilitySystemComponent->ClearActorInfo();
		}

		OnAbilitySystemUninitialized.Broadcast();
	}

	AbilitySystemComponent = nullptr;
}

void UOHDPawnComponentExtension::OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)//todo primary where it's used as commented!!!???
{
	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(Delegate);
	}

	if (AbilitySystemComponent)
	{
		Delegate.Execute();
	}
}

void UOHDPawnComponentExtension::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)//todo primary where it's used as commented!!!???
{
	if (!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemUninitialized.Add(Delegate);
	}
}
