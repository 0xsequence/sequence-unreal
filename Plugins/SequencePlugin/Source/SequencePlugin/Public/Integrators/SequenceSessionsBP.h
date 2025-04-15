// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SequenceAuthenticator.h"
#include "SequenceRPCManager.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "INativeAuthCallback.h"
#include "SequenceSessionsBP.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEmailLoginRequiresCode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEmailFederationRequiresCode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSessionEstablished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFederationSucceeded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFederationFailure);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSessionCreationFailure);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSignInWebViewRequired, const FString&, SignInUrl);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIdTokenReceived, const FString&, IdToken);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFederationRequired, const FFederationSupportData&, FederationData);

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceSessionsBP : public UGameInstanceSubsystem, public INativeAuthCallback
{
	GENERATED_BODY()
	
public:
	USequenceSessionsBP();
	
	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnEmailLoginRequiresCode EmailLoginRequiresCode;

	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnEmailFederationRequiresCode EmailFederationRequiresCode;
	
	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnSessionEstablished SessionEstablished;

	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnFederationSucceeded FederationSucceeded;

	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnFederationFailure FederationFailure;
	
	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnSessionCreationFailure SessionCreationFailure;

	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnSignInWebViewRequired SignInWebViewRequired;

	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnIdTokenReceived IdTokenReceived;

	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnFederationRequired FederationRequired;
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void StartEmailLoginAsync(const FString& Email);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void ConfirmEmailLoginWithCodeAsync(const FString& Code);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetGoogleTokenIdAsync();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetAppleTokenIdAsync();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void SignInWithGoogle(const FString& IdToken);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void SignInWithApple(const FString& IdToken);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void SignInWithEpic(const FString& IdToken);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void StartOidcSessionAsync(const FString& IdToken);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void PlayFabRegistrationAsync(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void PlayFabLoginAsync(const FString& UsernameIn, const FString& PasswordIn);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void PlayfabAuthenticateWithSessionTicketAsync(const FString& SessionTicket);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void StartGuestSessionAsync();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void FederateEmailAsync(const FString& EmailIn);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void ConfirmEmailFederationWithCodeAsync(const FString& Code);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void FederateOidcTokenAsync(const FString& IdTokenIn);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void FederatePlayFabRegistrationAsync(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn) const;

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void FederatePlayFabLoginAsync(const FString& UsernameIn, const FString& PasswordIn) const;

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void ClearSession() const;
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	bool CheckExistingSession() const;

	virtual void HandleNativeIdToken(const FString& IdToken) override;

private:
	UPROPERTY()
	USequenceRPCManager* RPCManager;
	
	UPROPERTY()
	USequenceAuthenticator* Authenticator;
	
	void StartSession(const FCredentials_BE& Credentials) const;
	void PlayFabNewAccountLoginRpcAsync(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);
	void PlayFabLoginRpcAsync(const FString& UsernameIn, const FString& PasswordIn, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);
	void PlayFabRpcAsync(const FString& Url, const FString& Content, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);

	void CallEmailLoginRequiresCode() const;
	void CallEmailFederationRequiresCode() const;
	void CallSessionEstablished() const;
	void CallFederationSucceeded() const;
	void CallFederationFailure() const;
	void CallSessionCreationFailure() const;
	void CallSignInWebViewRequired(const FString& SignInUrl) const;
	void CallIdTokenReceived(const FString& IdToken) const;
	void CallFederationRequired(const FFederationSupportData& FederationData) const;
};
