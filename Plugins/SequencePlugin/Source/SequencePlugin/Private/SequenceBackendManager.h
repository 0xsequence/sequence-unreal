// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Util/Structs/BE_Structs.h"
#include "Misc/AES.h"
#include "SequenceAuthenticator.h"
#include "SequenceBackendManager.generated.h"

class UIndexer;

USTRUCT(BlueprintType)
struct FUserDetails
{
	GENERATED_USTRUCT_BODY()
public:
	FString Username = "";
	FString Email = "";
	FString EmailService = "";
	int32 AccountID = -1;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuthIRequiresCode);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAuthIFailure, const FString&, Error);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuthISuccess);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFederateISuccess);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFederateIFailure, FString, Error);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIFederateOrForce, const FFederationSupportData&, FederationData);

UCLASS()
class SEQUENCEPLUGIN_API ASequenceBackendManager : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, Category="Auth")
		FOnAuthIRequiresCode ReadyToReceiveCodeDelegate;

	UPROPERTY(BlueprintAssignable, Category="Auth")
		FOnAuthIFailure ShowAuthFailureDelegate;

	UPROPERTY(BlueprintAssignable, Category="Auth")
		FOnAuthISuccess ShowAuthSuccessDelegate;

	UPROPERTY(BlueprintAssignable, Category="Federation")
		FOnFederateISuccess ShowFederateSuccessDelegate;

	UPROPERTY(BlueprintAssignable, Category="Federation")
		FOnFederateIFailure ShowFederationFailureDelegate;

	UPROPERTY(BlueprintAssignable, Category="Federation")
		FOnIFederateOrForce ShowFederateOrForceDelegate;
private:
	UFUNCTION()
		void CallReadyToReceiveCode();
	UFUNCTION()
		void CallShowAuthFailureScreen(const FString& ErrorIn);
	UFUNCTION()
		void CallShowAuthSuccessScreen();
	UFUNCTION()
		void CallShowFederationSuccess();
	UFUNCTION()
		void CallShowFederationFailure(const FString& Error);
	UFUNCTION()
		void CallShowFederateOrForce(const FFederationSupportData& FederationData);
private:	
	UPROPERTY()
	USequenceAuthenticator* Authenticator;

	//testing variables//
	bool bEnableTesting = true;
	TArray<FCoin_BE> TestCoins;
	TArray<FNFT_Master_BE> TestNfTs;
	//testing variables//

public:
	FUserDetails GetUserDetails();
	
	ASequenceBackendManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
//SYNC FUNCTIONAL CALLS// [THESE ARE BLOCKING CALLS AND WILL RETURN DATA IMMEDIATELY]

	UFUNCTION(BlueprintCallable, CATEGORY="FUNCTION")
		void SetupCustomEncryptor(UGenericNativeEncryptor * EncryptorIn);
	
	/*
	* Used to send data to clipboard for ease of use!
	*/
	UFUNCTION(BlueprintCallable, CATEGORY="FUNCTION")
		void CopyToClipboard(FString Data);

	/*
	* Used to get data from clipboard for ease of use
	*/
	UFUNCTION(BlueprintCallable, CATEGORY = "FUNCTION")
		FString GetFromClipboard();

	UFUNCTION(BlueprintCallable, CATEGORY = "FUNCTION")
		FString GetTransactionHash(FTransaction_FE Transaction);

	UFUNCTION(BlueprintCallable, Category="Login")
	void InitiateMobileSSO(const ESocialSigninType& Type, const bool ForceCreateAccountIn);

	UFUNCTION(BlueprintCallable, CATEGORY = "Login")
		FString GetLoginURL(const ESocialSigninType& Type);

	UFUNCTION(BlueprintCallable, CATEGORY = "Login")
		void SocialLogin(const FString& IDTokenIn, const bool ForceCreateAccountIn);

	UFUNCTION(BlueprintCallable, CATEGORY = "Login")
		void EmailLogin(const FString& EmailIn, const bool ForceCreateAccountIn);

	UFUNCTION(BlueprintCallable, CATEGORY = "Login")
		void EmailCode(const FString& CodeIn);

	UFUNCTION(BlueprintCallable, CATEGORY = "Login")
		void GuestLogin(const bool ForceCreateAccountIn);

	UFUNCTION(BlueprintCallable, CATEGORY = "Login")
		void PlayFabLogin(const FString& UsernameIn, const FString& PasswordIn, const bool ForceCreateAccountIn);

	UFUNCTION(BlueprintCallable, CATEGORY = "Login")
		void PlayFabRegisterAndLogin(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn, const bool ForceCreateAccountIn);
	
	UFUNCTION(BlueprintCallable, CATEGORY = "Federation")
		void FederateEmail(const FString& EmailIn) const;

	UFUNCTION(BlueprintCallable, CATEGORY = "Federation")
    	void FederateOIDCIdToken(const FString& IdTokenIn) const;

	UFUNCTION(BlueprintCallable, CATEGORY = "Federation")
    	void InitiateMobileFederateOIDC(const ESocialSigninType& TypeIn);

	UFUNCTION(BlueprintCallable, CATEGORY = "Federation")
    	void FederatePlayFabNewAccount(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn) const;

	UFUNCTION(BlueprintCallable, CATEGORY = "Federation")
    	void FederatePlayFabLogin(const FString& UsernameIn, const FString& PasswordIn) const;
	
	UFUNCTION(BlueprintCallable, Category = "Login")
		bool StoredCredentialsValid();
	
	UFUNCTION(BlueprintCallable, Category = "Login")
		FCredentials_BE GetStoredCredentials();

	UFUNCTION(BlueprintCallable, Category = "Login")
		void ForceOpenLastOpenSessionAttempt();

//SYNC FUNCTIONAL CALLS// [THESE ARE BLOCKING CALLS AND WILL RETURN DATA IMMEDIATELY]

//ASYNC FUNCTIONAL CALLS// [THESE ARE NON BLOCKING CALLS AND WILL USE A MATCHING UPDATE...FUNC TO RETURN DATA

	/*
	* Used to init. a call to fetch all system data needed by frontend in an async manner
	* Once system data struct is built we send it up with update_system_data(FSystemData_BE)
	*/
	UFUNCTION(BlueprintCallable, CATEGORY = "SystemData")
		void InitSystemData();

	/*
	* Used to update the frontend with the supplied system data
	* in an async manner
	*/
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, CATEGORY = "SystemData")
		void UpdateSystemData(const FSystemData_BE &systemData);

		void UpdateSystemTestableData(const FSystemData_BE& systemData);

	/*
	* Used to initalize a send transaction!
	*/
	UFUNCTION(BlueprintCallable, CATEGORY = "Send_Txn")
		void InitCoinSendTxn(FTransaction_FE TransactionData);

	UFUNCTION(BlueprintCallable, CATEGORY = "Send_Txn")
		void InitNFTSendTxn(FTransaction_FE TransactionData);

	/*
	* Used to let the frontend know if a txn went through or not!
	*/
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, CATEGORY = "Send_Txn")
		void UpdateTxn(FTxnCallback_BE TxnCallback);

	UFUNCTION(BlueprintCallable, CATEGORY = "Get_Updated_Data")
		void InitGetUpdatedCoinData(TArray<FID_BE> CoinsToUpdate);

	UFUNCTION(BlueprintCallable, CATEGORY = "Get_Updated_Data")
		void InitGetUpdateTokenData(TArray<FID_BE> TokensToUpdate);

//ASYNC FUNCTIONAL CALLS// [THESE ARE NON BLOCKING CALLS AND WILL USE A MATCHING UPDATE...FUNC TO RETURN DATA]
};