// Fill out your copyright notice in the Description page of Project Settings.


#include "ArrayFunctionLibrary.h"

auto UArrayFunctionLibrary::OrderByDistance(AActor* FromActor, TArray<AActor*> InActors) -> TArray<AActor*>
{
	auto predicate = [&](const AActor& A, const AActor& B)
	{
		return A.GetDistanceTo(FromActor) < B.GetDistanceTo(FromActor);
	};

	InActors.Sort(predicate);
	return InActors;
}
