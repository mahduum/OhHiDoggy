// Fill out your copyright notice in the Description page of Project Settings.


#include "DoggyComponent.h"

#include "CanineCharacter.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/Pawn.h"
#include "GameFramework//PlayerState.h"
#include "Misc/UObjectToken.h"
#include "OhHiDoggy/FOHDGameplayTags.h"
#include "OhHiDoggy/OHDLogChannels.h"
#include "OhHiDoggy/AbilitySystem/OHDAbilitySystemComponent.h"
#include "OhHiDoggy/Camera/OHDCameraComponent.h"
#include "OhHiDoggy/Camera/OHDCameraMode.h"
#include "OhHiDoggy/Components/OHDPawnComponentExtension.h"
#include "OhHiDoggy/Data/OHDPawnData.h"
#include "OhHiDoggy/Input/OHDInputConfig.h"
#include "OhHiDoggy/Input/OHDInputComponent.h"
#include "OhHiDoggy/Input/OHDInputModifiers.h"
#include "OhHiDoggy/Movement/CanineCharacterMovementComponent.h"
#include "OhHiDoggy/Player/OHDPlayerController.h"
#include "OhHiDoggy/Player/OHDPlayerState.h"
#include "OhHiDoggy/Settings/OHDSettingsLocal.h"
#include "OhHiDoggy/System/OHDAssetManager.h"

//#include "OhHiDoggy/Input/DoggyInputConfig.h"

namespace DoggyHero
{
	static const float LookYawRate = 300.0f;
	static const float LookPitchRate = 165.0f;
};

float UDoggyComponent::GetYawInputModifier() const
{
	return YawInputModifier;
}

void UDoggyComponent::SetYawInputModifier(float InValue)
{
	YawInputModifier = FMath::Clamp(InValue, 0.0f,1.0f);
}

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

	AOHDPlayerState* OhHiDoggyPS = GetPlayerState<AOHDPlayerState>();
	check(OhHiDoggyPS);
	
	UE_LOG(LogCore, Display, TEXT("Player state found."));


	const UOHDPawnData* PawnData = nullptr;

	 if (UOHDPawnComponentExtension* PawnExtComp = UOHDPawnComponentExtension::FindPawnExtensionComponent(Pawn))
	 {
	 	PawnData = PawnExtComp->GetPawnData<UOHDPawnData>();
	
	 	// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
	 	// The ability system component and attribute sets live on the player state.
	 	PawnExtComp->InitializeAbilitySystem(OhHiDoggyPS->GetOHDAbilitySystemComponent(), OhHiDoggyPS);
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

	if (UCanineCharacterMovementComponent* CanineMovement =
		UCanineCharacterMovementComponent::FindCanineMovementComponent(GetOwner()))
	{
		CanineMovement->OnMaxSpeedChanged.AddLambda([this](float InValue)
		{
			SetYawInputModifier(YawInputModifierCurve->GetFloatValue(InValue));//todo or use the same curve as for speeds but with different offset?
		});//AddUObject(this, &UDoggyComponent::SetYawInputModifier);
	}

	if (bIsLocallyControlled && PawnData)
	{
		if (UOHDCameraComponent* CameraComponent = UOHDCameraComponent::FindCameraComponent(Pawn))//todo
		{
			CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
		}
	}

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
				 	LocalSettings->OnInputConfigActivated.AddUObject(this, &UDoggyComponent::OnInputConfigActivated);//uobject will be called on broadcast of this delegate
				 	LocalSettings->OnInputConfigDeactivated.AddUObject(this, &UDoggyComponent::OnInputConfigDeactivated);
				 }

				//todo: create event and subscribe modifier to this event on binding???

				//todo what do to with those handles?:
				TArray<uint32> BindHandles;
				//input activated ability actions added in InputConfig as actions under AbilityInputActions - it is data asset class from UOHDInputConfig in DA_DoggyInputConfig_Agility) (examples: jump, reload, dash, fire, turn in place 90...
				DoggyIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

				/* input config is needed to find the actual InputAction asset in the game while all we are providing is just a tag, because input config has a helper
				 * function that allows to find it by tag given that inside the actual input config asset we had it set up correctly.
				 */
				DoggyIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
				DoggyIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
				DoggyIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
				// DoggyIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);//todo primary
				// DoggyIC->BindNativeAction(InputConfig, GameplayTags.InputTag_AutoRun, ETriggerEvent::Triggered, this, &ThisClass::Input_AutoRun, /*bLogIfNotFound=*/ false);//todo primary
			}
		}
	}
	
	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}
	//
	//UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);//todo primary
	//UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}


void UDoggyComponent::AddAdditionalInputConfig(const UOHDInputConfig* InputConfig)
{
	TArray<uint32> BindHandles;

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	UOHDInputComponent* OHDIC = Pawn->FindComponentByClass<UOHDInputComponent>();
	check(OHDIC);

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	if (const UOHDPawnComponentExtension* PawnExtComp = UOHDPawnComponentExtension::FindPawnExtensionComponent(Pawn))
	{
		OHDIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
	}
}

void UDoggyComponent::RemoveAdditionalInputConfig(const UOHDInputConfig* InputConfig)
{
	//@TODO: Implement me!
}

bool UDoggyComponent::HasPawnInitialized() const
{
	return bPawnHasInitialized;
}

bool UDoggyComponent::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
}

void UDoggyComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UOHDPawnComponentExtension* PawnExtComp = UOHDPawnComponentExtension::FindPawnExtensionComponent(Pawn))
		{
			if (UOHDAbilitySystemComponent* OHDASC = PawnExtComp->GetOHDAbilitySystemComponent())
			{
				OHDASC->AbilityInputTagPressed(InputTag);
			}
		}	
	}
}

void UDoggyComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	if (const UOHDPawnComponentExtension* PawnExtComp = UOHDPawnComponentExtension::FindPawnExtensionComponent(Pawn))
	{
		if (UOHDAbilitySystemComponent* OHDASC = PawnExtComp->GetOHDAbilitySystemComponent())
		{
			OHDASC->AbilityInputTagReleased(InputTag);
		}
	}
}

void UDoggyComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;
	
	// If the player has attempted to move again then cancel auto running
	if (AOHDPlayerController* OHDController = Cast<AOHDPlayerController>(Controller))
	{
		OHDController->SetIsAutoRunning(false);
	}
	
	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();

		const FRotator ControlMovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		
		auto TagLeft = FOHDGameplayTags::Get().InputTag_TurnLeft90.GetTagName();
		auto TagRight = FOHDGameplayTags::Get().InputTag_TurnRight90.GetTagName();

		// bool conatains = Pawn->Tags.ContainsByPredicate([&](const FName Tag) {return Tag == TagRight || Tag == TagLeft;});
		// UE_LOG(LogOHD, Display, TEXT("Pawn does not contain turning tags: %i."), conatains == false);

		//todo instead add tag as ability activation, ability will have this tags, how to avoid double ability for left and right?
		if (Value.X != 0 && Value.Y != 0)
		{
			Pawn->AddControllerYawInput(Value.X * GetYawInputModifier());//todo should rotate less? set variable rotation damper?
		}
		//todo link this ability to a separate input but ensure that it will never conflict with turn left while walking input (can activate etc.)
		else if (Value.X != 0 && Pawn->Tags.ContainsByPredicate([&](const FName Tag) {return Tag == TagRight || Tag == TagLeft;}) == false)
		{
			UE_LOG(LogOHD, Display, TEXT("Pawn does not contain turning tags."));
			if (Value.X > 0)
			{
				Pawn->Tags.Add(TagRight);
				UE_LOG(LogOHD, Display, TEXT("Added %s tag to pawn of type %s, tags count: %i."), *TagRight.ToString(), *Pawn->GetName(), Pawn->Tags.Num());
			}
			else
			{
				Pawn->Tags.Add(TagLeft);
				UE_LOG(LogOHD, Display, TEXT("Added %s tag to pawn of type %s, tags count: %i."), *TagLeft.ToString(), *Pawn->GetName(), Pawn->Tags.Num());
			}
			//testing ability here only temporary:
			
		}
		
		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = ControlMovementRotation.RotateVector(FVector::ForwardVector);//where is the forward vector pointing assuming that the character is always rotated relative to forward vector
			Pawn->AddMovementInput(MovementDirection, FMath::Clamp(Value.Y + FMath::Abs(Value.X), 0.0f, 1.0f));//todo separate two inputs to avoid this, and put a special modifier on turns to be accessed from gameplay
		}
	}
}


void UDoggyComponent::Input_LookMouse(const FInputActionValue& InputActionValue)//todo link it to camera boom or to control rig when looking/aiming
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		//Pawn->AddControllerYawInput(Value.X);//how sideways are we looking
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


void UDoggyComponent::Input_Crouch(const FInputActionValue& InputActionValue)
{
	if (ACanineCharacter* Character = GetPawn<ACanineCharacter>())
	{
		Character->ToggleCrouch();
	}
}

void UDoggyComponent::Input_AutoRun(const FInputActionValue& InputActionValue)
{
	if (APawn* Pawn = GetPawn<APawn>())
	{
		if (AOHDPlayerController* Controller = Cast<AOHDPlayerController>(Pawn->GetController()))
		{
			// Toggle auto running
			Controller->SetIsAutoRunning(!Controller->GetIsAutoRunning());
		}	
	}
}

TSubclassOf<UOHDCameraMode> UDoggyComponent::DetermineCameraMode() const
{
	if (AbilityCameraMode)
	{
		return AbilityCameraMode;
	}

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return nullptr;
	}

	if (UOHDPawnComponentExtension* PawnExtComp = UOHDPawnComponentExtension::FindPawnExtensionComponent(Pawn))
	{
		if (const UOHDPawnData* PawnData = PawnExtComp->GetPawnData<UOHDPawnData>())
		{
			return PawnData->DefaultCameraMode;
		}
	}

	return nullptr;
}

void UDoggyComponent::SetAbilityCameraMode(TSubclassOf<UOHDCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (CameraMode)
	{
		AbilityCameraMode = CameraMode;
		AbilityCameraModeOwningSpecHandle = OwningSpecHandle;
	}
}

void UDoggyComponent::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (AbilityCameraModeOwningSpecHandle == OwningSpecHandle)
	{
		AbilityCameraMode = nullptr;
		AbilityCameraModeOwningSpecHandle = FGameplayAbilitySpecHandle();
	}
}
