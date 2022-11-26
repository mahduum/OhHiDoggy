#pragma once

#include "CoreMinimal.h"
#include "Logging/LogMacros.h"

OHHIDOGGY_API DECLARE_LOG_CATEGORY_EXTERN(LogOHD, Log, All);
OHHIDOGGY_API DECLARE_LOG_CATEGORY_EXTERN(LogOHDExperience, Log, All);
OHHIDOGGY_API DECLARE_LOG_CATEGORY_EXTERN(LogOHDAbilitySystem, Log, All);
OHHIDOGGY_API DECLARE_LOG_CATEGORY_EXTERN(LogOHDTeams, Log, All);

OHHIDOGGY_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
