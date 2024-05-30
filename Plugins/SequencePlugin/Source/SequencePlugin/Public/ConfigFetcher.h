// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ConfigFetcher.generated.h"

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API UConfigFetcher : public UObject
{
	GENERATED_BODY()
public:
	//Config keys
	static inline FString FallBackEncryptionKey = "FallbackEncryptionKey";
	static inline FString WaaSTenantKey = "WaaSTenantKey";
	static inline FString ProjectAccessKey = "ProjectAccessKey";
	static inline FString GoogleClientID = "GoogleClientID";
	static inline FString AppleClientID = "AppleClientID";
	static inline FString FacebookClientID = "FacebookClientID";
	static inline FString DiscordClientID = "DiscordClientID";
	static inline FString RedirectUrl = "RedirectUrl";
	//Config Keys
	
	static FString GetConfigVar(const FString& ConfigKey);
};
