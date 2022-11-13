// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "OhHiDoggyPawnComponent.generated.h"

UINTERFACE(BlueprintType)
class OHHIDOGGY_API UOhHiDoggyReadyInterface : public UInterface
{
	GENERATED_BODY()
};

class OHHIDOGGY_API IOhHiDoggyReadyInterface
{
	GENERATED_BODY()
public:
	virtual bool IsPawnComponentReadyToInitialize() const = 0;
};

/**
 * 
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class OHHIDOGGY_API UOhHiDoggyPawnComponent : public UPawnComponent, public IOhHiDoggyReadyInterface
{
	GENERATED_BODY()
public:
	UOhHiDoggyPawnComponent(const FObjectInitializer& ObjectInitializer);

	virtual bool IsPawnComponentReadyToInitialize() const override { return true; }
};
