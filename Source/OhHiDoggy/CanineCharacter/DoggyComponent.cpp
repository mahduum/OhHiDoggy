// Fill out your copyright notice in the Description page of Project Settings.


#include "DoggyComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework//PlayerState.h"
#include "Misc/UObjectToken.h"
#include "OhHiDoggy/Components/OhHiDoggyPawnComponentExt.h"

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
		UE_LOG(LogCore, Error, TEXT("[ULyraHeroComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."));

//todo: what is it for?
#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("LyraHeroComponent", "NotOnPawnError", "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName HeroMessageLogName = TEXT("LyraHeroComponent");
			
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

	APlayerState* LyraPS = GetPlayerState<APlayerState>();
	check(LyraPS);

	//const ULyraPawnData* PawnData = nullptr;

	// if (ULyraPawnExtensionComponent* PawnExtComp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	// {
	// 	PawnData = PawnExtComp->GetPawnData<ULyraPawnData>();
	//
	// 	// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
	// 	// The ability system component and attribute sets live on the player state.
	// 	PawnExtComp->InitializeAbilitySystem(LyraPS->GetLyraAbilitySystemComponent(), LyraPS);
	// }

	if (APlayerController* LyraPC = GetController<APlayerController>())
	{
		if (Pawn->InputComponent != nullptr)
		{
			InitializePlayerInput(Pawn->InputComponent);
		}
	}

	// if (bIsLocallyControlled && PawnData)
	// {
	// 	if (ULyraCameraComponent* CameraComponent = ULyraCameraComponent::FindCameraComponent(Pawn))
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

	// if (const UDoggyPawnExtensionComponent* PawnExtComp = UDoggyPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	// {
	// 	if (const UDoggyPawnData* PawnData = PawnExtComp->GetPawnData<UDoggyPawnData>())
	// 	{
	// 		if (const UDoggyInputConfig* InputConfig = PawnData->InputConfig)
	// 		{
	// 			//const FDoggyGameplayTags& GameplayTags = FDoggyGameplayTags::Get();//todo make as ULyraGameplayTags
	//
	// 			// Register any default input configs with the settings so that they will be applied to the player during AddInputMappings
	// 			// for (const FMappableConfigPair& Pair : DefaultInputConfigs)
	// 			// {
	// 			// 	FMappableConfigPair::ActivatePair(Pair);
	// 			// }
	// 			
	// 			UDoggyInputComponent* DoggyIC = CastChecked<UDoggyInputComponent>(PlayerInputComponent);//todo
	//
	// 			DoggyIC->AddInputMappings(InputConfig, Subsystem);
	// 			if (UDoggySettingsLocal* LocalSettings = UDoggySettingsLocal::Get())
	// 			{
	// 				LocalSettings->OnInputConfigActivated.AddUObject(this, &UDoggyComponent::OnInputConfigActivated);
	// 				LocalSettings->OnInputConfigDeactivated.AddUObject(this, &UDoggyComponent::OnInputConfigDeactivated);
	// 			}
	//
	// 			TArray<uint32> BindHandles;
	// 			DoggyIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
	//
	// 			DoggyIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
	// 			DoggyIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
	// 			DoggyIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
	// 			DoggyIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);
	// 			DoggyIC->BindNativeAction(InputConfig, GameplayTags.InputTag_AutoRun, ETriggerEvent::Triggered, this, &ThisClass::Input_AutoRun, /*bLogIfNotFound=*/ false);
	// 		}
	// 	}
	// }
	//
	// if (ensure(!bReadyToBindInputs))
	// {
	// 	bReadyToBindInputs = true;
	// }
	//
	// UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	// UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}


void UDoggyComponent::Input_Move(const FInputActionValue& InputActionValue) const
{
	APawn* Pawn = GetPawn<APawn>();
	const AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	// If the player has attempted to move again then cancel auto running
	// if (ALyraPlayerController* LyraController = Cast<ALyraPlayerController>(Controller))
	// {
	// 	LyraController->SetIsAutoRunning(false);
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
