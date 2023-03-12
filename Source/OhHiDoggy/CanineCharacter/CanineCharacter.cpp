// Fill out your copyright notice in the Description page of Project Settings.

#include "CanineCharacter.h"

#include "GenericTeamAgentInterface.h"
#include "Components/GameFrameworkComponentManager.h"
#include "../Movement/CanineCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "OhHiDoggy/FOHDGameplayTags.h"
#include "../AbilitySystem/OHDAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "OhHiDoggy/Camera/OHDCameraComponent.h"
#include "OhHiDoggy/Player/OHDPlayerController.h"
#include "OhHiDoggy/Player/OHDPlayerState.h"


ACanineCharacter::ACanineCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCanineCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Avoid ticking characters if possible.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bUseControllerRotationYaw = 0;

	NetCullDistanceSquared = 900000000.0f;

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(36.0f, 38.0f);

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));  // Rotate mesh to be X forward since it is exported as Y forward.

	UCanineCharacterMovementComponent* CanineMoveComp = CastChecked<UCanineCharacterMovementComponent>(GetCharacterMovement());
	CanineMoveComp->GravityScale = 1.0f;
	CanineMoveComp->MaxAcceleration = 2400.0f;
	CanineMoveComp->BrakingFrictionFactor = 1.0f;
	CanineMoveComp->BrakingFriction = 6.0f;
	CanineMoveComp->GroundFriction = 8.0f;
	CanineMoveComp->BrakingDecelerationWalking = 1400.0f;
	CanineMoveComp->bUseControllerDesiredRotation = false;
	CanineMoveComp->bOrientRotationToMovement = false;
	CanineMoveComp->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	CanineMoveComp->bAllowPhysicsRotationDuringAnimRootMotion = false;
	CanineMoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	CanineMoveComp->bCanWalkOffLedgesWhenCrouching = true;
	CanineMoveComp->SetCrouchedHalfHeight(65.0f);

	
	PawnExtComponent = CreateDefaultSubobject<UOHDPawnComponentExtension>(TEXT("PawnExtensionComponent"));
	PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));


	// Temporary camera settings://todo remove when OHDCamera is ready
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	
	// TODO primary remember to set the custom camera like in OHD
	CameraComponent = CreateDefaultSubobject<UOHDCameraComponent>(TEXT("CameraComponent"));
	//CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

	// Create a follow camera //todo remove when OHDCamera is ready
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	BaseEyeHeight = 80.0f;
	CrouchedEyeHeight = 50.0f;
}

void ACanineCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UE_LOG(LogCore, Display, TEXT("Player input component setup."));//todo debug when it's happening
	
	PawnExtComponent->SetupPlayerInputComponent();
}

//todo move it higher to modular character base
void ACanineCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	/*Enable this to be able to interact with GameFramework and allow to be modified by GameFeatures or other plugins*/
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

//todo move it higher to modular character base
void ACanineCharacter::BeginPlay()
{
	/*Send a message to GameFramework that it is ready for extensions on part of GameFeature or other plugins*/
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::BeginPlay();

	// UWorld* World = GetWorld();
	//
	// const bool bRegisterWithSignificanceManager = !IsNetMode(NM_DedicatedServer);
	// if (bRegisterWithSignificanceManager)
	// {
	// 	if (UOHDSignificanceManager* SignificanceManager = USignificanceManager::Get<UOHDSignificanceManager>(World))
	// 	{
	// 		//@TODO: SignificanceManager->RegisterObject(this, (EFortSignificanceType)SignificanceType);
	// 	}
	// }
}

//todo move it higher to modular character base
void ACanineCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	/*Remove ability to receive extensions from plugins*/
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);

	//todo
	// UWorld* World = GetWorld();
	//
	// const bool bRegisterWithSignificanceManager = !IsNetMode(NM_DedicatedServer);
	// if (bRegisterWithSignificanceManager)
	// {
	// 	if (UOHDSignificanceManager* SignificanceManager = USignificanceManager::Get<UOHDSignificanceManager>(World))
	// 	{
	// 		SignificanceManager->UnregisterObject(this);
	// 	}
	// }
}

void ACanineCharacter::Reset()
{
	DisableMovementAndCollision();

	K2_OnReset();

	UninitAndDestroy();
}

void ACanineCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// DOREPLIFETIME_CONDITION(ThisClass, ReplicatedAcceleration, COND_SimulatedOnly);//todo medium
	// DOREPLIFETIME(ThisClass, MyTeamID)
}

void ACanineCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		// Compress Acceleration: XY components as direction + magnitude, Z component as direct value
		const double MaxAccel = MovementComponent->MaxAcceleration;
		const FVector CurrentAccel = MovementComponent->GetCurrentAcceleration();
		double AccelXYRadians, AccelXYMagnitude;
		FMath::CartesianToPolar(CurrentAccel.X, CurrentAccel.Y, AccelXYMagnitude, AccelXYRadians);

		//todo replicated acceleration
		// ReplicatedAcceleration.AccelXYRadians   = FMath::FloorToInt((AccelXYRadians / TWO_PI) * 255.0);     // [0, 2PI] -> [0, 255]
		// ReplicatedAcceleration.AccelXYMagnitude = FMath::FloorToInt((AccelXYMagnitude / MaxAccel) * 255.0);	// [0, MaxAccel] -> [0, 255]
		// ReplicatedAcceleration.AccelZ           = FMath::FloorToInt((CurrentAccel.Z / MaxAccel) * 127.0);   // [-MaxAccel, MaxAccel] -> [-127, 127]
	}
}

void ACanineCharacter::NotifyControllerChanged()
{
	//const FGenericTeamId OldTeamId = GetGenericTeamId();

	Super::NotifyControllerChanged();

	// Update our team ID based on the controller//todo team
	// if (HasAuthority() && (Controller != nullptr))
	// {
	// 	if (IOHDTeamAgentInterface* ControllerWithTeam = Cast<IOHDTeamAgentInterface>(Controller))
	// 	{
	// 		MyTeamID = ControllerWithTeam->GetGenericTeamId();
	// 		ConditionalBroadcastTeamChanged(this, OldTeamId, MyTeamID);
	// 	}
	// }
}

bool ACanineCharacter::CanTurnInPlace90() const
{
	return CanTurnInPlace90Internal();
}

bool ACanineCharacter::CanTurnInPlace90Internal_Implementation() const
{
	//todo expand?
	return !bIsCrouched && JumpIsAllowedInternal();
}

bool ACanineCharacter::TurnInPlace90IsAllowedInternal() const
{
	//todo primary do something similar but for turns, conditions will be similar though
	// Ensure that the CharacterMovement state is valid
	// bool bJumpIsAllowed = CharacterMovement->CanAttemptJump();
	//
	// if (bJumpIsAllowed)
	// {
	// 	// Ensure JumpHoldTime and JumpCount are valid.
	// 	if (!bWasJumping || GetJumpMaxHoldTime() <= 0.0f)
	// 	{
	// 		if (JumpCurrentCount == 0 && CharacterMovement->IsFalling())
	// 		{
	// 			bJumpIsAllowed = JumpCurrentCount + 1 < JumpMaxCount;
	// 		}
	// 		else
	// 		{
	// 			bJumpIsAllowed = JumpCurrentCount < JumpMaxCount;
	// 		}
	// 	}
	// 	else
	// 	{
	// 		// Only consider JumpKeyHoldTime as long as:
	// 		// A) The jump limit hasn't been met OR
	// 		// B) The jump limit has been met AND we were already jumping
	// 		const bool bJumpKeyHeld = (bPressedJump && JumpKeyHoldTime < GetJumpMaxHoldTime());
	// 		bJumpIsAllowed = bJumpKeyHeld &&
	// 			((JumpCurrentCount < JumpMaxCount) || (bWasJumping && JumpCurrentCount == JumpMaxCount));
	// 	}
	// }
	//
	// return bJumpIsAllowed;
	return true;
}

AOHDPlayerController* ACanineCharacter::GetOHDPlayerController() const
{
	return CastChecked<AOHDPlayerController>(Controller, ECastCheckedType::NullAllowed);
}

AOHDPlayerState* ACanineCharacter::GetOHDPlayerState() const
{
	return CastChecked<AOHDPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

UOHDAbilitySystemComponent* ACanineCharacter::GetOHDAbilitySystemComponent() const
{
	return Cast<UOHDAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* ACanineCharacter::GetAbilitySystemComponent() const
{
	return PawnExtComponent->GetOHDAbilitySystemComponent();
}

void ACanineCharacter::OnAbilitySystemInitialized()
{
	UOHDAbilitySystemComponent* OHDASC = GetOHDAbilitySystemComponent();
	check(OHDASC);

	//HealthComponent->InitializeWithAbilitySystem(OHDASC);//todo primary

	InitializeGameplayTags();
}

void ACanineCharacter::OnAbilitySystemUninitialized()
{
	//HealthComponent->UninitializeFromAbilitySystem();//todo primary
}

void ACanineCharacter::PossessedBy(AController* NewController)
{
	//const FGenericTeamId OldTeamID = MyTeamID;

	Super::PossessedBy(NewController);
	UE_LOG(LogCore, Warning, TEXT("Player is being possessed by Controller."));//todo debug when it's happening

	PawnExtComponent->HandleControllerChanged();

	// Grab the current team ID and listen for future changes
	// if (IOHDTeamAgentInterface* ControllerAsTeamProvider = Cast<IOHDTeamAgentInterface>(NewController))
	// {
	// 	MyTeamID = ControllerAsTeamProvider->GetGenericTeamId();
	// 	ControllerAsTeamProvider->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnControllerChangedTeam);
	// }
	// ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void ACanineCharacter::UnPossessed()
{
	AController* const OldController = Controller;

	// Stop listening for changes from the old controller
	// const FGenericTeamId OldTeamID = MyTeamID;
	// if (IOHDTeamAgentInterface* ControllerAsTeamProvider = Cast<IOHDTeamAgentInterface>(OldController))
	// {
	// 	ControllerAsTeamProvider->GetTeamChangedDelegateChecked().RemoveAll(this);
	// }

	Super::UnPossessed();

	PawnExtComponent->HandleControllerChanged();

	// Determine what the new team ID should be afterwards
	// MyTeamID = DetermineNewTeamAfterPossessionEnds(OldTeamID);
	// ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void ACanineCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	PawnExtComponent->HandleControllerChanged();
}

void ACanineCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//PawnExtComponent->HandlePlayerStateReplicated();//todo
}

void ACanineCharacter::InitializeGameplayTags()
{
	// Clear tags that may be lingering on the ability system from the previous pawn.
	if (UOHDAbilitySystemComponent* OHDASC = GetOHDAbilitySystemComponent())
	{
		const FOHDGameplayTags& GameplayTags = FOHDGameplayTags::Get();

		for (const TPair<uint8, FGameplayTag>& TagMapping : GameplayTags.MovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				OHDASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		for (const TPair<uint8, FGameplayTag>& TagMapping : GameplayTags.CustomMovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				OHDASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		UCanineCharacterMovementComponent* OHDMoveComp = CastChecked<UCanineCharacterMovementComponent>(GetCharacterMovement());
		SetMovementModeTag(OHDMoveComp->MovementMode, OHDMoveComp->CustomMovementMode, true);
	}
}

void ACanineCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UOHDAbilitySystemComponent* OHDASC = GetOHDAbilitySystemComponent())
	{
		OHDASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool ACanineCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UOHDAbilitySystemComponent* OHDASC = GetOHDAbilitySystemComponent())
	{
		return OHDASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool ACanineCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UOHDAbilitySystemComponent* OHDASC = GetOHDAbilitySystemComponent())
	{
		return OHDASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool ACanineCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UOHDAbilitySystemComponent* OHDASC = GetOHDAbilitySystemComponent())
	{
		return OHDASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

void ACanineCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	//HealthComponent->DamageSelfDestruct(/*bFellOutOfWorld=*/ true);//todo primary
}

void ACanineCharacter::OnDeathStarted(AActor*)
{
	DisableMovementAndCollision();
}

void ACanineCharacter::OnDeathFinished(AActor*)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}


void ACanineCharacter::DisableMovementAndCollision()//todo primary, why? death?
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UCanineCharacterMovementComponent* OHDMoveComp = CastChecked<UCanineCharacterMovementComponent>(GetCharacterMovement());
	OHDMoveComp->StopMovementImmediately();
	OHDMoveComp->DisableMovement();
}

void ACanineCharacter::DestroyDueToDeath()
{
	K2_OnDeathFinished();

	UninitAndDestroy();
}


void ACanineCharacter::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (UOHDAbilitySystemComponent* OHDASC = GetOHDAbilitySystemComponent())
	{
		if (OHDASC->GetAvatarActor() == this)
		{
			PawnExtComponent->UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}

void ACanineCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	UCanineCharacterMovementComponent* OHDMoveComp = CastChecked<UCanineCharacterMovementComponent>(GetCharacterMovement());

	SetMovementModeTag(PrevMovementMode, PreviousCustomMode, false);
	SetMovementModeTag(OHDMoveComp->MovementMode, OHDMoveComp->CustomMovementMode, true);
}

void ACanineCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
	if (UOHDAbilitySystemComponent* OHDASC = GetOHDAbilitySystemComponent())
	{
		const FOHDGameplayTags& GameplayTags = FOHDGameplayTags::Get();
		const FGameplayTag* MovementModeTag = nullptr;

		if (MovementMode == MOVE_Custom)
		{
			MovementModeTag = GameplayTags.CustomMovementModeTagMap.Find(CustomMovementMode);
		}
		else
		{
			MovementModeTag = GameplayTags.MovementModeTagMap.Find(MovementMode);
		}

		if (MovementModeTag && MovementModeTag->IsValid())
		{
			OHDASC->SetLooseGameplayTagCount(*MovementModeTag, (bTagEnabled ? 1 : 0));
		}
	}
}

void ACanineCharacter::ToggleCrouch()
{
	const UCanineCharacterMovementComponent* OHDMoveComp = CastChecked<UCanineCharacterMovementComponent>(GetCharacterMovement());

	if (bIsCrouched || OHDMoveComp->bWantsToCrouch)
	{
		UnCrouch();
	}
	else if (OHDMoveComp->IsMovingOnGround())
	{
		Crouch();
	}
}

void ACanineCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (UOHDAbilitySystemComponent* OHDASC = GetOHDAbilitySystemComponent())
	{
		OHDASC->SetLooseGameplayTagCount(FOHDGameplayTags::Get().Status_Crouching, 1);
	}


	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void ACanineCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (UOHDAbilitySystemComponent* OHDASC = GetOHDAbilitySystemComponent())
	{
		OHDASC->SetLooseGameplayTagCount(FOHDGameplayTags::Get().Status_Crouching, 0);
	}

	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

bool ACanineCharacter::CanJumpInternal_Implementation() const
{
	// same as ACharacter's implementation but without the crouch check
	return JumpIsAllowedInternal();
}

void ACanineCharacter::OnRep_ReplicatedAcceleration()
{
	if (UCanineCharacterMovementComponent* OHDMovementComponent = Cast<UCanineCharacterMovementComponent>(GetCharacterMovement()))
	{
		// Decompress Acceleration
		// const double MaxAccel         = OHDMovementComponent->MaxAcceleration;
		// const double AccelXYMagnitude = double(ReplicatedAcceleration.AccelXYMagnitude) * MaxAccel / 255.0; // [0, 255] -> [0, MaxAccel]
		// const double AccelXYRadians   = double(ReplicatedAcceleration.AccelXYRadians) * TWO_PI / 255.0;     // [0, 255] -> [0, 2PI]

		FVector UnpackedAcceleration(FVector::ZeroVector);
		// FMath::PolarToCartesian(AccelXYMagnitude, AccelXYRadians, UnpackedAcceleration.X, UnpackedAcceleration.Y);
		// UnpackedAcceleration.Z = double(ReplicatedAcceleration.AccelZ) * MaxAccel / 127.0; // [-127, 127] -> [-MaxAccel, MaxAccel]

		OHDMovementComponent->SetReplicatedAcceleration(UnpackedAcceleration);
	}
}
//
// void ACanineCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
// {
// 	if (GetController() == nullptr)
// 	{
// 		if (HasAuthority())
// 		{
// 			const FGenericTeamId OldTeamID = MyTeamID;
// 			MyTeamID = NewTeamID;
// 			ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
// 		}
// 		else
// 		{
// 			UE_LOG(LogOHDTeams, Error, TEXT("You can't set the team ID on a character (%s) except on the authority"), *GetPathNameSafe(this));
// 		}
// 	}
// 	else
// 	{
// 		UE_LOG(LogOHDTeams, Error, TEXT("You can't set the team ID on a possessed character (%s); it's driven by the associated controller"), *GetPathNameSafe(this));
// 	}
// }
//
// FGenericTeamId ACanineCharacter::GetGenericTeamId() const
// {
// 	return MyTeamID;
// }
//
// FOnOHDTeamIndexChangedDelegate* ACanineCharacter::GetOnTeamIndexChangedDelegate()
// {
// 	return &OnTeamChangedDelegate;
// }
//
// void ACanineCharacter::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
// {
// 	const FGenericTeamId MyOldTeamID = MyTeamID;
// 	MyTeamID = IntegerToGenericTeamId(NewTeam);
// 	ConditionalBroadcastTeamChanged(this, MyOldTeamID, MyTeamID);
// }
//
// void ACanineCharacter::OnRep_MyTeamID(FGenericTeamId OldTeamID)
// {
// 	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
// }

