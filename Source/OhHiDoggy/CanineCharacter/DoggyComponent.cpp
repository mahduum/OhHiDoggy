// Fill out your copyright notice in the Description page of Project Settings.


#include "DoggyComponent.h"

#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/Pawn.h"
#include "GameFramework//PlayerState.h"
#include "Misc/UObjectToken.h"
#include "OhHiDoggy/FOHDGameplayTags.h"
#include "OhHiDoggy/OHDLogChannels.h"
#include "OhHiDoggy/Components/OHDPawnComponentExtension.h"
#include "OhHiDoggy/Data/OHDPawnData.h"
#include "OhHiDoggy/Input/OHDInputConfig.h"
#include "OhHiDoggy/Input/OHDInputComponent.h"
#include "OhHiDoggy/Settings/OHDSettingsLocal.h"
#include "OhHiDoggy/System/OHDAssetManager.h"

//#include "OhHiDoggy/Input/DoggyInputConfig.h"

namespace DoggyHero
{
	static const float LookYawRate = 300.0f;
	static const float LookPitchRate = 165.0f;
};

void UDoggyComponent::OnRegister()
{
	Super::OnRegister();

	//todo before using pawn extension try using a character for testing?
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (UOHDPawnComponentExtension* PawnExtComp = UOHDPawnComponentExtension::FindPawnExtensionComponent(Pawn))
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
	UE_LOG(LogCore, Display, TEXT("Pawn is ready to initialize for input."));	

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

	UE_LOG(LogCore, Display, TEXT("Pawn is found for input to initialize on."));

	const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

	APlayerState* OhHiDoggyPS = GetPlayerState<APlayerState>();
	check(OhHiDoggyPS);
	
	UE_LOG(LogCore, Display, TEXT("Player state found."));


	const UOHDPawnData* PawnData = nullptr;

	 if (UOHDPawnComponentExtension* PawnExtComp = UOHDPawnComponentExtension::FindPawnExtensionComponent(Pawn))
	 {
	 	PawnData = PawnExtComp->GetPawnData<UOHDPawnData>();
	
	 	// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
	 	// The ability system component and attribute sets live on the player state.
	 	//PawnExtComp->InitializeAbilitySystem(OhHiDoggyPS->GetOhHiDoggyAbilitySystemComponent(), OhHiDoggyPS);
	 }

	if (APlayerController* OhHiDoggyPC = GetController<APlayerController>())
	{
		UE_LOG(LogCore, Display, TEXT("Controller is found for input to initialize on."));

		if (Pawn->InputComponent != nullptr)
		{
			UE_LOG(LogCore, Display, TEXT("Pawn's input component is ready to initialize player input."));

			InitializePlayerInput(Pawn->InputComponent);
		}
	}

	// if (bIsLocallyControlled && PawnData)
	// {
	// 	if (UOhHiDoggyCameraComponent* CameraComponent = UOhHiDoggyCameraComponent::FindCameraComponent(Pawn))//todo
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

	if (const UOHDPawnComponentExtension* PawnExtComp = UOHDPawnComponentExtension::FindPawnExtensionComponent(Pawn))
	{
		UE_LOG(LogCore, Display, TEXT("Pawn's extension component is ready for input init."));

		if (const UOHDPawnData* PawnData = PawnExtComp->GetPawnData<UOHDPawnData>())//todo make pawn data and nest it in extension comp
		{
			UE_LOG(LogCore, Display, TEXT("Pawn's data is ready for input init."));

			//input config is necessary to bind native input actions to functions in c++ code below, without it the native actions won't be bound to any functions
			//and we would not be able to trigger them
			if (const UOHDInputConfig* InputConfig = PawnData->InputConfig)
			{
				UE_LOG(LogCore, Display, TEXT("Input config found in pawn's data for input init."));

				//todo make as UOhHiDoggyGameplayTags, tags by which to find to what input action the native action is bound
				//an input action has its tag defined and assigned and it can by found by that tag, and internally then it bounds and action, object, trigger event with this input action.
				const FOHDGameplayTags& GameplayTags = FOHDGameplayTags::Get();
	
				 //Register any default input configs with the settings so that they will be applied to the player during AddInputMappings
				 for (const FMappableConfigPair& Pair : DefaultInputConfigs)
				 {
				 	FMappableConfigPair::ActivatePair(Pair);
				 }
				
				UOHDInputComponent* DoggyIC = CastChecked<UOHDInputComponent>(PlayerInputComponent);//todo
	
				DoggyIC->AddInputMappings(InputConfig, Subsystem);//todo primary this was disabled (all commented out)
				if (UOHDSettingsLocal* LocalSettings = UOHDSettingsLocal::Get())//todo primary local settings not existing!!! Use normal settings
				 {
				 	LocalSettings->OnInputConfigActivated.AddUObject(this, &UDoggyComponent::OnInputConfigActivated);
				 	LocalSettings->OnInputConfigDeactivated.AddUObject(this, &UDoggyComponent::OnInputConfigDeactivated);
				 }

				//todo with abilities ready:
				//TArray<uint32> BindHandles;
				//DoggyIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
	
				DoggyIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
				DoggyIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
				DoggyIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
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
	//UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);//todo
	//UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}


void UDoggyComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	const AController* Controller = Pawn ? Pawn->GetController() : nullptr;


	// If the player has attempted to move again then cancel auto running
	// if (AOhHiDoggyPlayerController* OhHiDoggyController = Cast<AOhHiDoggyPlayerController>(Controller))//todo
	// {
	// 	OhHiDoggyController->SetIsAutoRunning(false);
	// }
	
	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();

		if (Value.X != 0)//rotate controller first
		{
			//get velocity, divide input by velocity, each frame diminish velocity by 1? Or deal with it directly in BP? Modify rotation strength when it comes?
			//double LocalVelocityX = Pawn->GetActorRotation().UnrotateVector(Pawn->GetVelocity()).X;

			Pawn->AddControllerYawInput(Value.X);//todo value needs to be attenuated? try modifying it directly in input action by a dedicated modifier?
		}
		
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (bMoveSidewaysWithRotationInput && Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);//where is the right vector pointing
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);//where is the forward vector pointing assuming that the character is always rotated relative to forward vector
			Pawn->AddMovementInput(MovementDirection, Value.Y);
			//todo add rotation
		}
	}
}


void UDoggyComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);//how sideways are we looking
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);//how height are we looking 
	}
}

void UDoggyComponent::Input_LookStick(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	const UWorld* World = GetWorld();
	check(World);

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X * DoggyHero::LookYawRate * World->GetDeltaSeconds());
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y * DoggyHero::LookPitchRate * World->GetDeltaSeconds());
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
	if (APlayerController* OhHiDoggyPC = GetController<APlayerController>())//todo primary make oh hi doggy player controller
	{
		if (APawn* Pawn = GetPawn<APawn>())
		{
			if (UOHDInputComponent* OhHiDoggyIC = Cast<UOHDInputComponent>(Pawn->InputComponent))
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
			if (UOHDInputComponent* OhHiDoggyIC = Cast<UOHDInputComponent>(Pawn->InputComponent))
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
