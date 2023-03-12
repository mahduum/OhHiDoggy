﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilites/OHDGameplayAbility.h"
#include "UObject/Object.h"
#include "OHDGameplayAbility_TurnInPlace90.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class OHHIDOGGY_API UOHDGameplayAbility_TurnInPlace90 : public UOHDGameplayAbility
{
	GENERATED_BODY()


public:

	UOHDGameplayAbility_TurnInPlace90(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable, Category = "OHD|Ability")
	void CharacterTurnInPlace90Start();

	UFUNCTION(BlueprintCallable, Category = "OHD|Ability")
	void CharacterTurnInPlace90Stop();
};
