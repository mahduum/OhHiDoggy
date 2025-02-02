﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "OHDExperienceDefinition.h"


#define LOCTEXT_NAMESPACE "OHDSystem"

UOHDExperienceDefinition::UOHDExperienceDefinition()
{
}

#if WITH_EDITOR
EDataValidationResult UOHDExperienceDefinition::IsDataValid(TArray<FText>& ValidationErrors)
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(ValidationErrors), EDataValidationResult::Valid);

	int32 EntryIndex = 0;
	// todo gamefeatureaction
	// for (UGameFeatureAction* Action : Actions)
	// {
	// 	if (Action)
	// 	{
	// 		EDataValidationResult ChildResult = Action->IsDataValid(ValidationErrors);
	// 		Result = CombineDataValidationResults(Result, ChildResult);
	// 	}
	// 	else
	// 	{
	// 		Result = EDataValidationResult::Invalid;
	// 		ValidationErrors.Add(FText::Format(LOCTEXT("ActionEntryIsNull", "Null entry at index {0} in Actions"), FText::AsNumber(EntryIndex)));
	// 	}
	//
	// 	++EntryIndex;
	// }

	// Make sure users didn't subclass from a BP of this (it's fine and expected to subclass once in BP, just not twice)
	if (!GetClass()->IsNative())
	{
		UClass* ParentClass = GetClass()->GetSuperClass();

		// Find the native parent
		UClass* FirstNativeParent = ParentClass;
		while ((FirstNativeParent != nullptr) && !FirstNativeParent->IsNative())
		{
			FirstNativeParent = FirstNativeParent->GetSuperClass();
		}

		if (FirstNativeParent != ParentClass)
		{
			ValidationErrors.Add(FText::Format(LOCTEXT("ExperienceInheritenceIsUnsupported", "Blueprint subclasses of Blueprint experiences is not currently supported (use composition via ActionSets instead). Parent class was {0} but should be {1}."), 
				FText::AsCultureInvariant(GetPathNameSafe(ParentClass)),
				FText::AsCultureInvariant(GetPathNameSafe(FirstNativeParent))
			));
			Result = EDataValidationResult::Invalid;
		}
	}

	auto GetName = [] (EDataValidationResult InResult)-> const ANSICHAR*
	{
		switch (InResult){
		case EDataValidationResult::Invalid:
			return "Invalid";
		case EDataValidationResult::NotValidated:
			return "NotValidated";
		case EDataValidationResult::Valid:
			return "Valid";
		}
		return "";
	};

	UE_LOG(LogCore, Display, TEXT("Experience validation result: %hs."), GetName(Result));

	return Result;
}
#endif

#if WITH_EDITORONLY_DATA
void UOHDExperienceDefinition::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();

	// todo gamefeatureaction
	// for (UGameFeatureAction* Action : Actions)
	// {
	// 	if (Action)
	// 	{
	// 		Action->AddAdditionalAssetBundleData(AssetBundleData);
	// 	}
	// }
}
#endif // WITH_EDITORONLY_DATA

#undef LOCTEXT_NAMESPACE