// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueInterface.h"
#include "GameplayTagAssetInterface.h"
#include "GameFramework/Character.h"
#include "OhHiDoggy/Components/OHDPawnComponentExtension.h"
#include "AbilitySystemInterface.h"
#include "CanineCharacter.generated.h"

class UOHDCameraComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDoggyPossessed, UInputComponent*)

USTRUCT()
struct FCanineReplicatedAcceleration
{
	GENERATED_BODY()

	UPROPERTY()
	uint8 AccelXYRadians = 0;	// Direction of XY accel component, quantized to represent [0, 2*pi]

	UPROPERTY()
	uint8 AccelXYMagnitude = 0;	//Accel rate of XY component, quantized to represent [0, MaxAcceleration]

	UPROPERTY()
	int8 AccelZ = 0;	// Raw Z accel rate component, quantized to represent [-MaxAcceleration, MaxAcceleration]
};

class UAbilitySystemComponent;

//TODO: in the future make it derive from AModularCharacter, also copy abilities set, movement tags etc. from OHDCharacter
UCLASS(Config = Game, Meta = (ShortTooltip = "The base character pawn class used by this project."))
class ACanineCharacter : public ACharacter, public IAbilitySystemInterface, public IGameplayCueInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character *///todo is temp
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera *///todo is temp
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:

	ACanineCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "OHD|Character")
	AOHDPlayerController* GetOHDPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "OHD|Character")
	AOHDPlayerState* GetOHDPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "OHD|Character")
	UOHDAbilitySystemComponent* GetOHDAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	void ToggleCrouch();

	//~AActor interface
	virtual void PreInitializeComponents() override;//todo implement it in higher base AModularChararcter class with the logic to register with GameFeature
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Reset() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	//~End of AActor interface

	//~APawn interface
	virtual void NotifyControllerChanged() override;
	//~End of APawn interface

	//~IOHDTeamAgentInterface interface
	// virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	// virtual FGenericTeamId GetGenericTeamId() const override;
	// virtual FOnOHDTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of IOHDTeamAgentInterface interface

	UFUNCTION(BlueprintCallable, Category=Character)
	bool CanTurnInPlace90() const;

	UPROPERTY(BlueprintReadOnly, Category=Character)
	uint32 bPressedTurnInPlace90:1;

	UFUNCTION(BlueprintCallable, Category=Character)
	void TurnInPlace90();

	UFUNCTION(BlueprintCallable, Category=Character)
	void StopTurningInPlace90();

protected:
	/**
 * Customizable event to check if the character can jump in the current state.
 * Default implementation returns true if the character is on the ground and not crouching,
 * has a valid CharacterMovementComponent and CanEverJump() returns true.
 * Default implementation also allows for 'hold to jump higher' functionality: 
 * As well as returning true when on the ground, it also returns true when GetMaxJumpTime is more
 * than zero and IsJumping returns true.
 * 
 *
 * @Return Whether the character can jump in the current state. 
 */
	UFUNCTION(BlueprintNativeEvent, Category=Character, meta=(DisplayName="CanTurnInPlace90"))
	bool CanTurnInPlace90Internal() const;
	virtual bool CanTurnInPlace90Internal_Implementation() const;
	bool TurnInPlace90IsAllowedInternal() const;
	
	//todo use from PawnComponentExt instead
	FOnDoggyPossessed OnDoggyPossessedDelegate;

	virtual void OnAbilitySystemInitialized();
	virtual void OnAbilitySystemUninitialized();

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void OnRep_Controller() override;
	virtual void OnRep_PlayerState() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void InitializeGameplayTags();

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	// Begins the death sequence for the character (disables collision, disables movement, etc...)
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	// Ends the death sequence for the character (detaches controller, destroys pawn, etc...)
	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);

	void DisableMovementAndCollision();
	void DestroyDueToDeath();
	void UninitAndDestroy();

	// Called when the death sequence for the character has completed
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnDeathFinished"))
	void K2_OnDeathFinished();

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual bool CanJumpInternal_Implementation() const;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OHD|Character", Meta = (AllowPrivateAccess = "true"))
	UOHDPawnComponentExtension* PawnExtComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OHD|Character", Meta = (AllowPrivateAccess = "true"))
	UOHDCameraComponent* CameraComponent;//todo
	//

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OHD|Character", Meta = (AllowPrivateAccess = "true"))
	// UOHDHealthComponent* HealthComponent;//todo primary

	// UPROPERTY(Transient, ReplicatedUsing = OnRep_ReplicatedAcceleration)
	// FOHDReplicatedAcceleration ReplicatedAcceleration;

	// UPROPERTY(ReplicatedUsing = OnRep_MyTeamID)
	// FGenericTeamId MyTeamID;
	//
	// UPROPERTY()
	// FOnOHDTeamIndexChangedDelegate OnTeamChangedDelegate;

// protected:
// 	// Called to determine what happens to the team ID when possession ends
// 	virtual FGenericTeamId DetermineNewTeamAfterPossessionEnds(FGenericTeamId OldTeamID) const
// 	{
// 		// This could be changed to return, e.g., OldTeamID if you want to keep it assigned afterwards, or return an ID for some neutral faction, or etc...
// 		return FGenericTeamId::NoTeam;
// 	}

private:
	// UFUNCTION()
	// void OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

	UFUNCTION()
	void OnRep_ReplicatedAcceleration();

	// UFUNCTION()
	// void OnRep_MyTeamID(FGenericTeamId OldTeamID);
};

