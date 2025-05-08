// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "eos_sdk.h"
#include "eos_auth.h"
#include "eos_connect.h"
#include "EOSManager.generated.h"

UCLASS(Blueprintable)
class SEQUENCEUNREAL_API UEOSManager : public UObject
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCallbackValue, const FString&, SignedMessage);
	DECLARE_DYNAMIC_DELEGATE(FOnCallback);

public:
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK")
	static UEOSManager* MakeEosManager();

#pragma region EOS_SDK
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK")
	void Tick();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK")
	void Initialize();

private:
	EOS_HPlatform EosPlatform = nullptr;

#pragma endregion

#pragma region Epic_Account_Services
	
public:
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK")
	void LoginWithEpicAccountServices(FOnCallback OnSuccess, FOnCallback OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK")
	void CopyEpicAccountAccessToken(FOnCallbackValue OnSuccess, FOnCallback OnFailure);

private:
	EOS_HAuth EosAuth = nullptr;
	EOS_EpicAccountId EpicAccountId = nullptr;
	
#pragma endregion

#pragma region Connect_Services
	
public:
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK")
	void LoginWithConnectServices(const FString& ExternalToken, FOnCallback OnSuccess, FOnCallback OnFailure);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK")
	void CopyConnectIdToken(FOnCallbackValue OnSuccess, FOnCallback OnFailure);

private:
	EOS_HConnect EosConnect = nullptr;
	EOS_ProductUserId ProductUserId = nullptr;

#pragma endregion
};
