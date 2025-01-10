// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if COMMONUSER_OSSV1

// Online Subsystem (OSS v1) includes and forward declares
#include "OnlineSubsystemTypes.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
class IOnlineSubsystem;
struct FOnlineError;
using FOnlineErrorType = FOnlineError;
using ELoginStatusType = ELoginStatus::Type;

#else

// Online Services (OSS v2) includes and forward declares
#include "Online/Connectivity.h"
#include "Online/OnlineError.h"
namespace UE::Online
{
	enum class ELoginStatus : uint8;
	enum class EPrivilegeResults : uint32;
	enum class EUserPrivileges : uint8;
	using IAuthPtr = TSharedPtr<class IAuth>;
	using IOnlineServicesPtr = TSharedPtr<class IOnlineServices>;
	template <typename OpType>
	class TOnlineResult;
	struct FAuthLogin;
	struct FConnectionStatusChanged;
	struct FExternalUIShowLoginUI;
	struct FLoginStatusChanged;
	struct FQueryUserPrivilege;
	class FAccountInfo;
}
using FOnlineErrorType = UE::Online::FOnlineError;
using ELoginStatusType = UE::Online::ELoginStatus;

#endif

#include "CommonUserTypes.generated.h"


/** Enum specifying where and how to run online queries */
UENUM(BlueprintType)
enum class ECommonUserOnlineContext : uint8
{
	/** Called from game code, this uses the default system but with special handling that could merge results from multiple contexts */
	Game,

	/** The default engine online system, this will always exist and will be the same as either Service or Platform */
	Default,
	
	/** Explicitly ask for the external service, which may not exist */
	Service,

	/** Looks for external service first, then falls back to default */
	ServiceOrDefault,
	
	/** Explicitly ask for the platform system, which may not exist */
	Platform,

	/** Looks for platform system first, then falls back to default */
	PlatformOrDefault,

	/** Invalid system */
	Invalid
};

/** Enum describing the state of initialization for a specific user */
UENUM(BlueprintType)
enum class ECommonUserInitializationState : uint8
{
	/** User has not started login process */
	Unknown,

	/** Player is in the process of acquiring a user id with local login */
	DoingInitialLogin,

	/** Player is performing the network login, they have already logged in locally */
	DoingNetworkLogin,

	/** Player failed to log in at all */
	FailedtoLogin,

	
	/** Player is logged in and has access to online functionality */
	LoggedInOnline,

	/** Player is logged in locally (either guest or real user), but cannot perform online actions */
	LoggedInLocalOnly,


	/** Invalid state or user */
	Invalid,
};

/** Enum specifying different privileges and capabilities available to a user */
UENUM(BlueprintType)
enum class ECommonUserPrivilege : uint8
{
	/** Whether the user can play at all, online or offline */
	CanPlay,

	/** Whether the user can play in online modes */
	CanPlayOnline,

	/** Whether the user can use text chat */
	CanCommunicateViaTextOnline,

	/** Whether the user can use voice chat */
	CanCommunicateViaVoiceOnline,

	/** Whether the user can access content generated by other users */
	CanUseUserGeneratedContent,

	/** Whether the user can ever participate in cross-play */
	CanUseCrossPlay,

	/** Invalid privilege (also the count of valid ones) */
	Invalid_Count					UMETA(Hidden)
};

/** Enum specifying the general availability of a feature or privilege, this combines information from multiple sources */
UENUM(BlueprintType)
enum class ECommonUserAvailability : uint8
{
	/** State is completely unknown and needs to be queried */
	Unknown,

	/** This feature is fully available for use right now */
	NowAvailable,

	/** This might be available after the completion of normal login procedures */
	PossiblyAvailable,

	/** This feature is not available now because of something like network connectivity but may be available in the future */
	CurrentlyUnavailable,

	/** This feature will never be available for the rest of this session due to hard account or platform restrictions */
	AlwaysUnavailable,

	/** Invalid feature */
	Invalid,
};

/** Enum giving specific reasons why a user may or may not use a certain privilege */
UENUM(BlueprintType)
enum class ECommonUserPrivilegeResult : uint8
{
	/** State is unknown and needs to be queried */
	Unknown,

	/** This privilege is fully available for use */
	Available,

	/** User has not fully logged in */
	UserNotLoggedIn,

	/** User does not own the game or content */
	LicenseInvalid,

	/** The game needs to be updated or patched before this will be available */
	VersionOutdated,

	/** No network connection, this may be resolved by reconnecting */
	NetworkConnectionUnavailable,

	/** Parental control failure */
	AgeRestricted,

	/** Account does not have a required subscription or account type */
	AccountTypeRestricted,

	/** Another account/user restriction such as being banned by the service */
	AccountUseRestricted,

	/** Other platform-specific failure */
	PlatformFailure,
};

/** Used to track the progress of different asynchronous operations */
enum class ECommonUserAsyncTaskState : uint8
{
	/** The task has not been started */
	NotStarted,
	/** The task is currently being processed */
	InProgress,
	/** The task has completed successfully */
	Done,
	/** The task failed to complete */
	Failed
};
