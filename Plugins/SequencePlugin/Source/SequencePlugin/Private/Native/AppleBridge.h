// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "AppleBridge.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API UAppleBridge : public UObject
{
	GENERATED_BODY()

public:
	UAppleBridge();
	static void InitiateGoogleSSO(const FString& Url, const FString& Scheme, void(*IOSCallback)(char *idToken));
	static void InitiateIosSSO(const FString& clientID, const FString& nonce, void(*IOSCallback)(char *idToken));

	static FString AppleEncrypt(const FString& StringIn);
	static FString AppleDecrypt(const FString& StringIn);
};