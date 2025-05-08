
#pragma once
#include "CoreMinimal.h"
#include "PlayFabSendIntent.generated.h"

USTRUCT()
struct SEQUENCEPLUGIN_API FPlayFabPlayerProfileViewConstraints
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	bool ShowAvatarUrl = false;
	UPROPERTY()
	bool ShowBannedUntil = false;
	UPROPERTY()
	bool ShowCampaignAttributions = false;
	UPROPERTY()
	bool ShowContactEmailAddresses = false;
	UPROPERTY()
	bool ShowCreated = false;
	UPROPERTY()
	bool ShowDisplayName = false;
	UPROPERTY()
	bool ShowExperimentVariants = false;
	UPROPERTY()
	bool ShowLastLogin = false;
	UPROPERTY()
	bool ShowLinkedAccounts = false;
	UPROPERTY()
	bool ShowLocations = false;
	UPROPERTY()
	bool ShowMemberships = false;
	UPROPERTY()
	bool ShowOrigination = false;
	UPROPERTY()
	bool ShowPushNotificationRegistrations = false;
	UPROPERTY()
	bool ShowStatistics = false;
	UPROPERTY()
	bool ShowTags = false;
	UPROPERTY()
	bool ShowTotalValueToDateInUsd = false;
	UPROPERTY()
	bool ShowValuesToDate = false;
};

USTRUCT()
struct SEQUENCEPLUGIN_API FPlayFabInfoRequestParameters
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	bool GetCharacterInventories = false;
	UPROPERTY()
	bool GetCharacterList = false;
	UPROPERTY()
	bool GetPlayerProfile = false;
	UPROPERTY()
	bool GetPlayerStatistics = false;
	UPROPERTY()
	bool GetTitleData = false;
	UPROPERTY()
	bool GetUserAccountInfo = false;
	UPROPERTY()
	bool GetUserData = false;
	UPROPERTY()
	bool GetUserInventory = false;
	UPROPERTY()
	bool GetUserReadOnlyData = false;
	UPROPERTY()
	bool GetUserVirtualCurrency = false;
	UPROPERTY()
	TArray<FString> PlayerStatisticNames;
	UPROPERTY()
	FPlayFabPlayerProfileViewConstraints ProfileConstraints;
	UPROPERTY()
	TArray<FString> TitleDataKeys;
	UPROPERTY()
	TArray<FString> UserDataKeys;
	UPROPERTY()
	TArray<FString> UserReadOnlyDataKeys;
};

USTRUCT()
struct SEQUENCEPLUGIN_API FPlayFabRegisterUser
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FString TitleId = "";
	UPROPERTY()
	FString Email = "";
	UPROPERTY()
	FPlayFabInfoRequestParameters InfoRequestParameters;
	UPROPERTY()
	FString Password = "";
	UPROPERTY()
	bool RequireBothUsernameAndEmail = true;
	UPROPERTY()
	FString Username = "";

	FPlayFabRegisterUser(){}
	
	FPlayFabRegisterUser(const FString& TitleIdIn, const FString& EmailIn, const FString& PasswordIn, const FString& UsernameIn)
	{
		TitleId = TitleIdIn;
		Email = EmailIn;
		Password = PasswordIn;
		Username = UsernameIn;
	}
};

USTRUCT()
struct SEQUENCEPLUGIN_API FPlayFabLoginUser
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FString Password = "";
	UPROPERTY()
	FString TitleId = "";
	UPROPERTY()
	FString Username = "";

	FPlayFabLoginUser(){}

	FPlayFabLoginUser(const FString& PasswordIn, const FString& TitleIdIn, const FString& UsernameIn)
	{
		Password = PasswordIn;
		TitleId = TitleIdIn;
		Username = UsernameIn;
	}
};