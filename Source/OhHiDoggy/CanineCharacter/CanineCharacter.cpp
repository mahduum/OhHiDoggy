// Fill out your copyright notice in the Description page of Project Settings.

#include "CanineCharacter.h"
#include "Components/GameFrameworkComponentManager.h"
#include "../Movement/CanineCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

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

	// CameraComponent = CreateDefaultSubobject<ULyraCameraComponent>(TEXT("CameraComponent"));
	// CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));

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

void ACanineCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
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
}

//todo move it higher to modular character base
void ACanineCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	/*Remove ability to receive extensions from plugins*/
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}

