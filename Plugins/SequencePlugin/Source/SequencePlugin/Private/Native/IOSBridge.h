// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "SequencePlugin/Public/Authenticator.h"
#include "CoreMinimal.h"
#include "IOSBridge.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API UIOSBridge : public UObject
{
	GENERATED_BODY()

public:
	UIOSBridge();
	static void InitiateGoogleSSO(const FString& Url, const FString& Scheme, void(*IOSCallback)(char *idToken));
	static void InitiateIosSSO(const FString& clientID, const FString& nonce, void(*IOSCallback)(char *idToken));
};