// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayEffectTypes.h"
#include "OhHiDoggy/OHDLogChannels.h"
#include "OhHiDoggy/Movement/CanineCharacterMovementComponent.h"
#include "CanineAnimInstance.generated.h"

// USTRUCT()
// struct FGroundMovementModeData
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, Category="Min Threshold for Ground Movement Mode", meta=(ForceUnits="cm/s"))
// 	float Min;
//
// 	UPROPERTY(EditAnywhere, Category="Max Threshold for Ground Movement Mode", meta=(ForceUnits="cm/s"))
// 	float Max;
//
// 	inline bool IsInRange(float CurrentSpeed) const
// 	{
// 		return CurrentSpeed >= Min && CurrentSpeed <= Max;
// 	}
// 	
	// friend uint32 GetTypeHash(const FGroundMovementModeData& InMyStruct)
	// {
	// 	return FCrc::MemCrc32(&InMyStruct, sizeof(FGroundMovementModeData));
	// }

	// friend uint32 GetTypeHash(const FGroundMovementModeData& InMyStruct)
	// {
	// 	return InMyStruct.GroundMovementMode;
	// }
	//
	// inline bool operator==(const FGroundMovementModeData& other) const
	// {
	// 	return GroundMovementMode == other.GroundMovementMode;
	// }

	// inline bool operator==(const FGroundMovementModeData& other) const
	// {
	// 	return other.Min >= Min && other.Max <= Max;
	// }
//};

// struct FGroundMovementModeKeyFuncs : TDefaultMapKeyFuncs<FGroundMovementModeData, ECanineGroundMovement, false>
// {
// 	static FORCEINLINE ECanineGroundMovement GetKey(const FGroundMovementModeData& Element)
// 	{
// 		return Element.GroundMovementMode;
// 	}
//
// 	static FORCEINLINE bool Matches(ECanineGroundMovement A, ECanineGroundMovement B)
// 	{
// 		return A == B;
// 	}
//
// 	static FORCEINLINE uint32 GetKeyHash(ECanineGroundMovement Key)
// 	{
// 		return GetTypeHash(Key);
// 	}
// };

UCLASS(BlueprintType, meta = (BlueprintThreadSafe))
class UGroundMovementModesCurve : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Speed Ranges for Ground Movement Modes")
	TArray<TEnumAsByte<ECanineGroundMovement>> GroundMovementModes;

	UPROPERTY(EditAnywhere, Category="Ground Movement Modes Curve")
	UCurveFloat* GroundMovementModesCurve;
};

/**
 * 
 */
UCLASS()
class UCanineAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);

protected:

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
#endif // WITH_EDITOR

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

	// Gameplay tags that can be mapped to blueprint variables. The variables will automatically update as the tags are added or removed.
	// These should be used instead of manually querying for the gameplay tags.
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;

	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	float GroundDistance = -1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ground Movement Modes Curve", meta = (BlueprintThreadSafe))
	UGroundMovementModesCurve* GroundMovementModes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ground Movement Modes Curve", meta = (BlueprintThreadSafe))
	UCurveFloat* GroundMovementModesCurve;

	UFUNCTION(BlueprintCallable, Category="Speed Ranges for Ground Movement Modes", meta = (AnimBlueprintFunction, BlueprintThreadSafe))
	void GetGroundMovementMode(float CurrentSpeed, TEnumAsByte<ECanineGroundMovement>& CanineGroundMovementMode, float& StrideWarpAlphaMinusOneToOne) const;
};
