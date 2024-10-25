// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SequenceAuthenticator.h"
#include "SequenceRPCManager.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SequenceSessionsBP.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEmailLoginRequiresCode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSessionEstablished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionCreationFailure, const FString&, Error);

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceSessionsBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	USequenceSessionsBP();
	
	UPROPERTY(BlueprintAssignable, Category="Sequence")
	FOnEmailLoginRequiresCode EmailLoginRequiresCode;

	UPROPERTY(BlueprintAssignable, Category="Sequence")
	FOnSessionEstablished SessionEstablished;
	
	UPROPERTY(BlueprintAssignable, Category="Sequence")
	FOnSessionCreationFailure SessionCreationFailure;
	
	UFUNCTION(BlueprintCallable, Category="Sequence")
	void StartEmailLoginAsync(const FString& Email);

	UFUNCTION(BlueprintCallable, Category="Sequence")
	void ConfirmEmailLoginWithCodeAsync(const FString& Code);

	UFUNCTION(BlueprintCallable, Category="Sequence")
	void GetGoogleTokenIdAsync();

	UFUNCTION(BlueprintCallable, Category="Sequence")
	void GetAppleTokenIdAsync();

	UFUNCTION(BlueprintCallable, Category="Sequence")
	void StartOidcSessionAsync(const FString& IdToken);

	UFUNCTION(BlueprintCallable, Category="Sequence")
	void PlayFabRegistrationAsync(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn);

	UFUNCTION(BlueprintCallable, Category="Sequence")
	void PlayFabLoginAsync(const FString& UsernameIn, const FString& PasswordIn);

	UFUNCTION(BlueprintCallable, Category="Sequence")
	void StartGuestSessionAsync();

	UFUNCTION(BlueprintCallable, Category="Sequence")
	void ClearSession();

private:
	USequenceRPCManager* RPCManager;
	USequenceAuthenticator* Authenticator;
	
	static USequenceSessionsBP* GetSubSystem();
	void StartSession(const FCredentials_BE& Credentials) const;
	void PlayFabNewAccountLoginRpcAsync(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);
	void PlayFabLoginRpcAsync(const FString& UsernameIn, const FString& PasswordIn, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);
	void PlayFabRpcAsync(const FString& Url, const FString& Content, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);

	void CallEmailLoginRequiresCode() const;
	void CallSessionEstablished() const;
	void CallSessionCreationFailure(const FString& Error) const;
};
