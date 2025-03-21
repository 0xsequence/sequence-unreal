// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SequenceRPCManager.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "INativeAuthCallback.h"
#include "Sequence/SequenceSessions.h"
#include "SequenceSessionsBP.generated.h"

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceSessionsBP : public UGameInstanceSubsystem, public INativeAuthCallback
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE(FOnSuccess);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnBrowserRequired, const FString&, SignInUrl);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFederationRequired, const FFederationSupportData&, FederationData);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFailure, const FString&, Error);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSession);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIdTokenReceived, const FString&, IdToken);
	
public:
	USequenceSessionsBP();

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnSession OnSessionCreated;
	
	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnIdTokenReceived IdTokenReceived;

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetGoogleTokenId(FOnBrowserRequired BrowserRequired);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetAppleTokenId(FOnBrowserRequired BrowserRequired);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	bool CheckExistingSession();
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void StartEmailLogin(const FString& Email, FOnSuccess RequiresCode, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void ConfirmEmailLoginWithCode(const FString& Code, FOnSuccess OnSuccess, FOnFailure OnFailure, FOnFederationRequired OnFederationRequired);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void StartOidcSession(const FString& IdToken, FOnSuccess OnSuccess, FOnFailure OnFailure, FOnFederationRequired OnFederationRequired);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void PlayFabRegistration(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn, FOnSuccess OnSuccess, FOnFailure OnFailure, FOnFederationRequired OnFederationRequired);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void PlayFabLogin(const FString& UsernameIn, const FString& PasswordIn, FOnSuccess OnSuccess, FOnFailure OnFailure, FOnFederationRequired OnFederationRequired);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void PlayFabAuthenticateWithSessionTicket(const FString& SessionTicket, FOnSuccess OnSuccess, FOnFailure OnFailure, FOnFederationRequired OnFederationRequired);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void StartGuestSession(FOnSuccess OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void FederateEmail(const FString& EmailIn, FOnSuccess OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void ConfirmEmailFederationWithCode(const FString& Code, FOnSuccess OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void FederateOidcToken(const FString& IdTokenIn, FOnSuccess OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void FederatePlayFabRegistration(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn, FOnSuccess OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void FederatePlayFabLogin(const FString& UsernameIn, const FString& PasswordIn, FOnSuccess OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void SetForceCreateAccount(const bool NewForceCreateAccount);
	
	virtual void HandleNativeIdToken(const FString& IdToken) override;

private:
	UPROPERTY()
	USequenceSessions* Sessions;
	
	void CallOnSessionCreated() const;
};
