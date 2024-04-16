// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
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
	//Config Keys
	
	static FString GetConfigVar(const FString& ConfigKey);
};
