// Fill out your copyright notice in the Description page of Project Settings.


#include "OHDPawnComponent.h"

UOHDPawnComponent::UOHDPawnComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}
