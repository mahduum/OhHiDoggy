// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ArrayFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UArrayFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category = Helpers)
	static TArray<AActor*> OrderByDistance(AActor* FromActor, TArray<AActor*> To);
};
