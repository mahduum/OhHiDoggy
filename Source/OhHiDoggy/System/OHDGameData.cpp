// Fill out your copyright notice in the Description page of Project Settings.


#include "OHDGameData.h"

#include "OHDAssetManager.h"

UOHDGameData::UOHDGameData()
{
}

const UOHDGameData& UOHDGameData::UOHDGameData::Get()
{
	return UOHDAssetManager::Get().GetGameData();
}