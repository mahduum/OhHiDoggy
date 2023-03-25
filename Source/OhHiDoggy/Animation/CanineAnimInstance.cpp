// Fill out your copyright notice in the Description page of Project Settings.
#include "CanineAnimInstance.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"


#if WITH_EDITOR
void UCanineAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);

	UE_LOG(LogOHDAbilitySystem, Display, TEXT("Initializing gameplay tag property map for anim instance."))
	GameplayTagPropertyMap.Initialize(this, ASC);
}

EDataValidationResult UCanineAnimInstance::IsDataValid(TArray<FText>& ValidationErrors)
{
	Super::IsDataValid(ValidationErrors);

	GameplayTagPropertyMap.IsDataValid(this, ValidationErrors);

	return ((ValidationErrors.Num() > 0) ? EDataValidationResult::Invalid : EDataValidationResult::Valid);
}
#endif // WITH_EDITOR

void UCanineAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AActor* OwningActor = GetOwningActor())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			UE_LOG(LogOHD, Display, TEXT("Initializing ability system from actor."));
			InitializeWithAbilitySystem(ASC);
		}
	}
}

void UCanineAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const ACharacter* Character = Cast<ACharacter>(GetOwningActor());
	if (!Character)
	{
		return;
	}

	// UCharacterMovementComponent* CharMoveComp = CastChecked<UCharacterMovementComponent>(Character->GetCharacterMovement());
	// const FLyraCharacterGroundInfo& GroundInfo = CharMoveComp->GetGroundInfo();//todo
	// GroundDistance = GroundInfo.GroundDistance;
}
//TODO also return float value for each mode, within space -1 to 1, first for play rate decrease later for increase, and abs value for stride warping alpha
void UCanineAnimInstance::GetGroundMovementMode(float CurrentSpeed, TEnumAsByte<ECanineGroundMovement>& CanineGroundMovementMode, float& StrideWarpAlphaMinusOneToOne) const
{
	TArray<TEnumAsByte<ECanineGroundMovement>> Modes = GroundMovementModes->GroundMovementModes;
	uint8 Index = 0;
	if (FMath::IsNearlyZero(CurrentSpeed))
	{
		CanineGroundMovementMode = Modes[Index];
		StrideWarpAlphaMinusOneToOne = 0.0f;
		return;
	}

	Index++;
	const float StepSize = 1.0f/Modes.Num();
	float StepSum = StepSize;
	const float Value = GroundMovementModes->GroundMovementModesCurve->GetFloatValue(CurrentSpeed);

	constexpr int LoopGuard = 100;
	while (Value > StepSum && Index < LoopGuard)//if value is greater then we are in the next step
	{
		StepSum += StepSize;
		Index++;
	}

	if (Index == LoopGuard)
	{
		UE_LOG(LogOHD, Error, TEXT("Loop guard break for speed ranges!"));
	}

	float RemapMinusOneToOne = 0.0f;
	if (Index < Modes.Num())
	{
		//get frac of step size
		const float StepLerp = Value - (StepSum - StepSize);
		const float StepPercent = UKismetMathLibrary::NormalizeToRange(StepLerp, 0.0f, StepSize);
		RemapMinusOneToOne = UKismetMathLibrary::MapRangeClamped(StepPercent, 0.0f, 1.f, -1.f, 1.f);
		// or: float RemapMinusOneToOne = (StepPercent - 0.5f) * 2.f;
	}
	else
	{
		RemapMinusOneToOne = 1.f;
	}
	
	CanineGroundMovementMode = Modes[Index < Modes.Num() ? Index : Modes.Last()];
	StrideWarpAlphaMinusOneToOne = RemapMinusOneToOne;
}

float UCanineAnimInstance::GetGroundMovementModeOptimalSpeedBasedOnCurrentSpeed(float CurrentSpeed) const
{
	TEnumAsByte<ECanineGroundMovement> CurrentCanineGroundMovementMode;
	float StrideWarpAlphaMinusOneToOne;
	GetGroundMovementMode(CurrentSpeed, CurrentCanineGroundMovementMode, StrideWarpAlphaMinusOneToOne);
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECanineGroundMovement"), true);
	const auto Mode = EnumPtr->GetNameByIndex((int32)CurrentCanineGroundMovementMode);
	UE_LOG(LogOHD, Display, TEXT("Current ground movement mode from speed is %s"), *Mode.ToString())
	TArray<FRichCurveKey> MiddleSpeedKeys = GroundMovementModes->GroundMovementModesCurve->FloatCurve.Keys;
	for (const auto& Key : MiddleSpeedKeys)
	{
		//todo cache for optimization
		TEnumAsByte<ECanineGroundMovement> GroundMovementModeForKey;
		float Unused;
		
		GetGroundMovementMode(Key.Time, GroundMovementModeForKey, Unused);

		const auto KeyMode = EnumPtr->GetNameByIndex((int32)GroundMovementModeForKey);
		UE_LOG(LogOHD, Display, TEXT("Key ground movement mode from speed is %s"), *KeyMode.ToString())

		if (GroundMovementModeForKey == CurrentCanineGroundMovementMode)
		{
			return Key.Time;
		}
	}

	return 0.0f;
}
