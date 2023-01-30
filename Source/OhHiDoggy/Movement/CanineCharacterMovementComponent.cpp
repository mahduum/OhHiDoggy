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

//todo remove interp and set desired gears from root motion speed
bool UCanineCharacterMovementComponent::TryChangeCurrentMaxSpeed(float SpeedDelta, float SpeedChangeRate)//todo speed delta can increase in value the longer it is being fed
{
	//TODO get max possible speed from movement component instead:
	constexpr float MaxPossibleSpeed = 1320.0f;

	if (FMath::IsNearlyEqual(MaxWalkSpeed, MaxPossibleSpeed, 0.0001))
	{
		return false;
	}

	//if not decelerating/accelerating slowly interpolate to the middle speed but elsewhere?
	//todo target speed to interp to will be the middle speed of current node, movement comp can provide it???
	//const float NewSpeed = FMath::FInterpTo(CurrentSpeed, TargetSpeed, GetWorld()->DeltaTimeSeconds, SpeedChangeRate);

	const float NewSpeed = FMath::Clamp(MaxWalkSpeed + SpeedDelta * SpeedChangeRate, 0.0f, MaxPossibleSpeed);

	MaxWalkSpeed = NewSpeed;
	OnMaxSpeedChanged.Broadcast(MaxWalkSpeed);
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
