// Fill out your copyright notice in the Description page of Project Settings.


#include "DoggyComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework//PlayerState.h"
#include "Misc/UObjectToken.h"
#include "OhHiDoggy/FOhHiDoggyGameplayTags.h"
#include "OhHiDoggy/Components/OhHiDoggyPawnComponentExt.h"
#include "OhHiDoggy/Data/OhHiDoggyPawnData.h"
#include "OhHiDoggy/Input/DoggyInputConfig.h"
#include "OhHiDoggy/Input/OhHiDoggyInputComponent.h"
#include "OhHiDoggy/Settings/OhHiDoggySettingsLocal.h"

//#include "OhHiDoggy/Input/DoggyInputConfig.h"

void UDoggyComponent::OnRegister()
{
	Super::OnRegister();

	//todo before using pawn extension try using a character for testing?
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (UOhHiDoggyPawnComponentExt* PawnExtComp = UOhHiDoggyPawnComponentExt::FindPawnExtensionComponent(Pawn))
		{
			PawnExtComp->OnPawnReadyToInitialize_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnPawnReadyToInitialize));
		}
	}
	else
	{
		UE_LOG(LogCore, Error, TEXT("[UOhHiDoggyHeroComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."));

//todo: what is it for?
#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("OhHiDoggyHeroComponent", "NotOnPawnError", "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName HeroMessageLogName = TEXT("OhHiDoggyHeroComponent");
			
			FMessageLog(HeroMessageLogName).Error()
				->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
				->AddToken(FTextToken::Create(Message));
				
			FMessageLog(HeroMessageLogName).Open();
		}
#endif
	}
	//get owner and add delegate initialize player input to owner to be fired on setup input 
	//InitializePlayerInput(GetController<APlayerController>()->GetPawn()->InputComponent);//todo crashes
}

void UDoggyComponent::OnPawnReadyToInitialize()//todo bound this in on register to owner or pawn ext when pawn will be ready to initialize (like onpossessed etc.)
{
	if (!ensure(!bPawnHasInitialized))
	{
		// Don't initialize twice
		return;
	}

	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}
	const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

	APlayerState* OhHiDoggyPS = GetPlayerState<APlayerState>();
	check(OhHiDoggyPS);

	//const UOhHiDoggyPawnData* PawnData = nullptr;

	// if (UOhHiDoggyPawnExtensionComponent* PawnExtComp = UOhHiDoggyPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	// {
	// 	PawnData = PawnExtComp->GetPawnData<UOhHiDoggyPawnData>();
	//
	// 	// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
	// 	// The ability system component and attribute sets live on the player state.
	// 	PawnExtComp->InitializeAbilitySystem(OhHiDoggyPS->GetOhHiDoggyAbilitySystemComponent(), OhHiDoggyPS);
	// }

	if (APlayerController* OhHiDoggyPC = GetController<APlayerController>())
	{
		if (Pawn->InputComponent != nullptr)
		{
			InitializePlayerInput(Pawn->InputComponent);
		}
	}

	// if (bIsLocallyControlled && PawnData)
	// {
	// 	if (UOhHiDoggyCameraComponent* CameraComponent = UOhHiDoggyCameraComponent::FindCameraComponent(Pawn))
	// 	{
	// 		CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
	// 	}
	// }

	bPawnHasInitialized = true;
}

void UDoggyComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UDoggyComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UDoggyComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	UE_LOG(LogCore, Error, TEXT("Initializing input on [%s]."), *GetNameSafe(this));

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	//todo add inherited from enhanced

	if (const UOhHiDoggyPawnComponentExt* PawnExtComp = UOhHiDoggyPawnComponentExt::FindPawnExtensionComponent(Pawn))
	{
		if (const UOhHiDoggyPawnData* PawnData = PawnExtComp->GetPawnData<UOhHiDoggyPawnData>())//todo make pawn data and nest it in extension comp
		{
			if (const UDoggyInputConfig* InputConfig = PawnData->InputConfig)//todo what input config
			{
				//todo make as UOhHiDoggyGameplayTags, tags by which to find to what input action the native action is bound
				//an input action has its tag defined and assigned and it can by found by that tag, and internally then it bounds and action, object, trigger event with this input action.
				const FOhHiDoggyGameplayTags& GameplayTags = FOhHiDoggyGameplayTags::Get();
	
				 //Register any default input configs with the settings so that they will be applied to the player during AddInputMappings
				 for (const FMappableConfigPair& Pair : DefaultInputConfigs)
				 {
				 	FMappableConfigPair::ActivatePair(Pair);
				 }
				
				UOhHiDoggyInputComponent* DoggyIC = CastChecked<UOhHiDoggyInputComponent>(PlayerInputComponent);//todo
	
				DoggyIC->AddInputMappings(InputConfig, Subsystem);
				if (UOhHiDoggySettingsLocal* LocalSettings = UOhHiDoggySettingsLocal::Get())
				{
					LocalSettings->OnInputConfigActivated.AddUObject(this, &UDoggyComponent::OnInputConfigActivated);
					LocalSettings->OnInputConfigDeactivated.AddUObject(this, &UDoggyComponent::OnInputConfigDeactivated);
				}

				//todo with abilities ready:
				//TArray<uint32> BindHandles;
				//DoggyIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
	
				DoggyIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
				// DoggyIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
				// DoggyIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
				// DoggyIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);
				// DoggyIC->BindNativeAction(InputConfig, GameplayTags.InputTag_AutoRun, ETriggerEvent::Triggered, this, &ThisClass::Input_AutoRun, /*bLogIfNotFound=*/ false);
			}
		}
	}
	
	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}
	//
	// UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	// UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}


void UDoggyComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	const AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	// If the player has attempted to move again then cancel auto running
	// if (AOhHiDoggyPlayerController* OhHiDoggyController = Cast<AOhHiDoggyPlayerController>(Controller))
	// {
	// 	OhHiDoggyController->SetIsAutoRunning(false);
	// }
	
	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}


bool UDoggyComponent::IsPawnComponentReadyToInitialize() const
{
	// The player state is required.
	if (!GetPlayerState<APlayerState>())
	{
		return false;
	}

	const APawn* Pawn = GetPawn<APawn>();

	// A pawn is required.
	if (!Pawn)
	{
		return false;
	}

	//If we're authority or autonomous, we need to wait for a controller with registered ownership of the player state.
	if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
	{
		AController* Controller = GetController<AController>();
	
		const bool bHasControllerPairedWithPS = (Controller != nullptr) && \
												(Controller->PlayerState != nullptr) && \
												(Controller->PlayerState->GetOwner() == Controller);
	
		if (!bHasControllerPairedWithPS)
		{
			return false;
		}
	}
	
	const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
	const bool bIsBot = Pawn->IsBotControlled();

	if (bIsLocallyControlled && !bIsBot)
	{
		// The input component is required when locally controlled.
		if (!Pawn->InputComponent)
		{
			return false;
		}
	}

	return true;
}


void UDoggyComponent::OnInputConfigActivated(const FLoadedMappableConfigPair& ConfigPair)
{
	if (APlayerController* OhHiDoggyPC = GetController<APlayerController>())//todo make oh hi doggy player controller
	{
		if (APawn* Pawn = GetPawn<APawn>())
		{
			if (UOhHiDoggyInputComponent* OhHiDoggyIC = Cast<UOhHiDoggyInputComponent>(Pawn->InputComponent))
			{
				if (const ULocalPlayer* LP = OhHiDoggyPC->GetLocalPlayer())
				{
					if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
					{
						OhHiDoggyIC->AddInputConfig(ConfigPair, Subsystem);	
					}	
				}
			}
		}
	}
}

void UDoggyComponent::OnInputConfigDeactivated(const FLoadedMappableConfigPair& ConfigPair)
{
	if (APlayerController* OhHiDoggyPC = GetController<APlayerController>())//todo make oh hi doggy player controller
	{
		if (APawn* Pawn = GetPawn<APawn>())
		{
			if (UOhHiDoggyInputComponent* OhHiDoggyIC = Cast<UOhHiDoggyInputComponent>(Pawn->InputComponent))
			{
				if (const ULocalPlayer* LP = OhHiDoggyPC->GetLocalPlayer())
				{
					if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
					{
						OhHiDoggyIC->RemoveInputConfig(ConfigPair, Subsystem);
					}
				}
			}
		}
	}
}
