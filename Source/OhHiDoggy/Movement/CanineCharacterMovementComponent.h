// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "NativeGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CanineCharacterMovementComponent.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_MovementStopped);

/**
 * FLyraCharacterGroundInfo
 *
 *	Information about the ground under the character.  It only gets updated as needed.
 */
USTRUCT(BlueprintType)
struct FCanineCharacterGroundInfo
{
	GENERATED_BODY()

	FCanineCharacterGroundInfo()
		: LastUpdateFrame(0)
		, GroundDistance(0.0f)
	{}

	uint64 LastUpdateFrame;

	UPROPERTY(BlueprintReadOnly)
	FHitResult GroundHitResult;

	UPROPERTY(BlueprintReadOnly)
	float GroundDistance;
};

/** Movement modes for Characters. */
UENUM(BlueprintType)
enum ECanineGroundMovement
{
	CANINE_MOVE_WalkingSlowly		UMETA(DisplayName="Canine Walk Slow"),
	
	CANINE_MOVE_Walking				UMETA(DisplayName="Canine Walk"),

	CANINE_MOVE_Trotting			UMETA(DisplayName="Canine Trot"),

	CANINE_MOVE_Running				UMETA(DisplayName="Canine Run"),
	
	CANINE_MOVE_Sprinting			UMETA(DisplayName="Canine Sprint"),

	CANINE_MOVE_WalkingBackwards	UMETA(DisplayName="Canine Walk Back"),
};

/**
 * 
 */
UCLASS(Config = Game)
class UCanineCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:

    UCanineCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

    virtual void SimulateMovement(float DeltaTime) override;

    virtual bool CanAttemptJump() const override;

    // Returns the current ground info.  Calling this will update the ground info if it's out of date.
    UFUNCTION(BlueprintCallable, Category = "OHD|CharacterMovement")
    const FCanineCharacterGroundInfo& GetGroundInfo();

	UFUNCTION(BlueprintCallable, Category = "OHD|CharacterMovement")
	static UCanineCharacterMovementComponent* FindCanineMovementComponent (const AActor* Actor)
	{
		return Actor ? Actor->FindComponentByClass<UCanineCharacterMovementComponent>() : nullptr;
	}

    void SetReplicatedAcceleration(const FVector& InAcceleration);

    //~UMovementComponent interface
    virtual FRotator GetDeltaRotation(float DeltaTime) const override;
    virtual float GetMaxSpeed() const override;
    //~End of UMovementComponent interface

	DECLARE_EVENT_OneParam(UCanineCharacterMovementComponent, FDelegateMaxSpeedChanged, float);
	FDelegateMaxSpeedChanged OnMaxSpeedChanged;

protected:

    virtual void InitializeComponent() override;

protected:

    // Cached ground info for the character.  Do not access this directly!  It's only updated when accessed via GetGroundInfo().
    FCanineCharacterGroundInfo CachedGroundInfo;

    UPROPERTY(Transient)
    bool bHasReplicatedAcceleration = false;

	UFUNCTION(BlueprintCallable, Category = "Canine Movement Modes")
	bool TryChangeCurrentMaxSpeed(float SpeedDelta, float SpeedChangeRate);
};
