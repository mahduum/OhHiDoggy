// Fill out your copyright notice in the Description page of Project Settings.
#include "CanineCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "CollisionQueryParams.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemGlobals.h"
#include "NativeGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "OhHiDoggy/OHDLogChannels.h"
#include <type_traits>

#include "Kismet2/EnumEditorUtils.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_MovementStopped, "Gameplay.MovementStopped");

namespace CanineCharacter
{
	static float GroundTraceDistance = 100000.0f;
	FAutoConsoleVariableRef CVar_GroundTraceDistance(TEXT("CanineCharacter.GroundTraceDistance"), GroundTraceDistance, TEXT("Distance to trace down when generating ground information."), ECVF_Cheat);
};


UCanineCharacterMovementComponent::UCanineCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MaxWalkSpeed = 0.0f;
}

void UCanineCharacterMovementComponent::SimulateMovement(float DeltaTime)
{
	if (bHasReplicatedAcceleration)
	{
		// Preserve our replicated acceleration
		const FVector OriginalAcceleration = Acceleration;
		Super::SimulateMovement(DeltaTime);
		Acceleration = OriginalAcceleration;
	}
	else
	{
		Super::SimulateMovement(DeltaTime);
	}
}

bool UCanineCharacterMovementComponent::CanAttemptJump() const
{
	// Same as UCharacterMovementComponent's implementation but without the crouch check
	return IsJumpAllowed() &&
		(IsMovingOnGround() || IsFalling()); // Falling included for double-jump and non-zero jump hold time, but validated by character.
}

void UCanineCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

TMap<TEnumAsByte<ECanineGroundMovement>, float> UCanineCharacterMovementComponent::GetCanineSpeeds() const
{
	return CanineMaxSpeeds;
}

TEnumAsByte<ECanineGroundMovement> UCanineCharacterMovementComponent::GetNextMode(const AActor* Actor, const UInputAction* InputAction)
{
	const UCanineCharacterMovementComponent* MovementComponent = Actor ? Actor->FindComponentByClass<UCanineCharacterMovementComponent>() : nullptr;
	if(MovementComponent == nullptr)
	{
		UE_LOG(LogOHD, Error, TEXT("Could not find %s"), *UCanineCharacterMovementComponent::StaticClass()->GetName());
		return ECanineGroundMovement::CANINE_MOVE_None;
	}
	//todo primary: change to doggy input action and use tags, get the bind to tag object to retrieve the tag from input
	//use TArray<FDoggyInputAction> NativeInputActions; from OHDInputConfig.h
	FString Name;
	InputAction->GetName(Name);
	UE_LOG(LogOHD, Display, TEXT("Input action name: %s"), *Name);

	//todo delete
	const UEnum* enumObject = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECanineGroundMovement"));
	const auto enumCount = enumObject->NumEnums();
	UE_LOG(LogOHD, Display, TEXT("Cannine enum count: %i"), enumCount);

	
	if(Name == "IA_AccelerateOverride")
	{
		if (MovementComponent->CurrentGroundMovementMode == ECanineGroundMovement::CANINE_MOVE_Sprinting)
		{
			return CANINE_MOVE_Sprinting;
		}

		const ECanineGroundMovement enumValue = MovementComponent->CurrentGroundMovementMode.GetValue();
		UE_LOG(LogOHD, Display, TEXT("Enum value: %i"), enumValue);
		std::underlying_type_t<ECanineGroundMovement> i = enumValue;
		i++;
		UE_LOG(LogOHD, Display, TEXT("Incremented i: %i"), i);

		return static_cast<ECanineGroundMovement>(i);
	}

	if(Name == "IA_DecelerateOverride")
	{
		if (MovementComponent->CurrentGroundMovementMode == ECanineGroundMovement::CANINE_MOVE_None)
		{
			return CANINE_MOVE_None;
		}

		const ECanineGroundMovement enumValue = MovementComponent->CurrentGroundMovementMode.GetValue();
		UE_LOG(LogOHD, Display, TEXT("Enum value: %i"), enumValue);
		std::underlying_type_t<ECanineGroundMovement> i = enumValue;
		i--;
		UE_LOG(LogOHD, Display, TEXT("Incremented i: %i"), i);

		return static_cast<ECanineGroundMovement>(i);
	}

	return CANINE_MOVE_None;
}

bool UCanineCharacterMovementComponent::TryChangeCurrentMaxSpeed(const UInputAction* InputAction, float SpeedChangeRate)
{
	const float CurrentSpeed = MaxWalkSpeed;//todo if current speed is greater than target... then? set the speed for the first time, hardcode 0 until changed, if is in motion then cache the speed, on mode changed clear the speed
	const TEnumAsByte<ECanineGroundMovement> NextMode = GetNextMode(GetOwner(), InputAction);
	const float TargetSpeed = *CanineMaxSpeeds.Find(NextMode);
	const float NewSpeed = FMath::FInterpTo(CurrentSpeed, TargetSpeed, GetWorld()->DeltaTimeSeconds, SpeedChangeRate);
	UE_LOG(LogOHD, Display, TEXT("Current Mode: %s"), *UEnum::GetValueAsString(CurrentGroundMovementMode));
	UE_LOG(LogOHD, Display, TEXT("Next Mode: %s"), *UEnum::GetValueAsString(NextMode));
	UE_LOG(LogOHD, Display, TEXT("Current Speed: %f"), CurrentSpeed);
	UE_LOG(LogOHD, Display, TEXT("Target Speed: %f"), TargetSpeed);

	if(FMath::IsNearlyZero(FMath::Abs(NewSpeed - TargetSpeed), 0.001))
	{
		CurrentGroundMovementMode = NextMode;
		MaxWalkSpeed = TargetSpeed;
		return false;
	}

	MaxWalkSpeed = NewSpeed;
	return true;
}

const FCanineCharacterGroundInfo& UCanineCharacterMovementComponent::GetGroundInfo()
{
	if (!CharacterOwner || (GFrameCounter == CachedGroundInfo.LastUpdateFrame))
	{
		return CachedGroundInfo;
	}

	if (MovementMode == MOVE_Walking)
	{
		CachedGroundInfo.GroundHitResult = CurrentFloor.HitResult;
		CachedGroundInfo.GroundDistance = 0.0f;
	}
	else
	{
		const UCapsuleComponent* CapsuleComp = CharacterOwner->GetCapsuleComponent();
		check(CapsuleComp);

		const float CapsuleHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
		const ECollisionChannel CollisionChannel = (UpdatedComponent ? UpdatedComponent->GetCollisionObjectType() : ECC_Pawn);
		const FVector TraceStart(GetActorLocation());
		const FVector TraceEnd(TraceStart.X, TraceStart.Y, (TraceStart.Z - CanineCharacter::GroundTraceDistance - CapsuleHalfHeight));

		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(CanineCharacterMovementComponent_GetGroundInfo), false, CharacterOwner);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(QueryParams, ResponseParam);

		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CollisionChannel, QueryParams, ResponseParam);

		CachedGroundInfo.GroundHitResult = HitResult;
		CachedGroundInfo.GroundDistance = CanineCharacter::GroundTraceDistance;

		if (MovementMode == MOVE_NavWalking)
		{
			CachedGroundInfo.GroundDistance = 0.0f;
		}
		else if (HitResult.bBlockingHit)
		{
			CachedGroundInfo.GroundDistance = FMath::Max((HitResult.Distance - CapsuleHalfHeight), 0.0f);
		}
	}

	CachedGroundInfo.LastUpdateFrame = GFrameCounter;

	return CachedGroundInfo;
}

void UCanineCharacterMovementComponent::SetReplicatedAcceleration(const FVector& InAcceleration)
{
	bHasReplicatedAcceleration = true;
	Acceleration = InAcceleration;
}

FRotator UCanineCharacterMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		if (ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped))
		{
			return FRotator(0,0,0);
		}
	}

	return Super::GetDeltaRotation(DeltaTime);
}

float UCanineCharacterMovementComponent::GetMaxSpeed() const
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		if (ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped))
		{
			return 0;
		}
	}

	return Super::GetMaxSpeed();
}
