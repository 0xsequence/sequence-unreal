// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Http.h"
#include "ObjectHandler.h"
#include "Indexer/Backend.h"
#include "Util/Structs/BE_Structs.h"
#include "Util/Structs/BE_Enums.h"
#include "Misc/AES.h"
#include "Auth.h"
#include "Sequence/SequenceAPI.h"
#include "Authenticator.h"
#include "SequenceBackendManager.generated.h"

class UIndexer;


//Move this to Structs.h
USTRUCT(BlueprintType)
struct FUserDetails
{
	GENERATED_USTRUCT_BODY()
public:
	FString username = "";
	FString email = "";
	FString emailService = "";
	int32 accountID = -1;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuthIRequiresCode);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuthIFailure);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuthISuccess);

UCLASS()
class SEQUENCEPLUGIN_API ASequenceBackendManager : public AActor, public IBackend
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
		FOnAuthIRequiresCode ReadyToReceiveCodeDelegate;

	UPROPERTY(BlueprintAssignable)
		FOnAuthIFailure ShowAuthFailureDelegate;

	UPROPERTY(BlueprintAssignable)
		FOnAuthISuccess ShowAuthSuccessDelegate;
private:
	UFUNCTION()
		void CallReadyToReceiveCode();
	UFUNCTION()
		void CallShowAuthFailureScreen();
	UFUNCTION()
		void CallShowAuthSuccessScreen(const FCredentials_BE& CredentialsIn);
private:	
	SequenceAPI::FSequenceWallet* sequenceWallet = nullptr;
	UIndexer* Indexer;
	FCredentials_BE Credentials;
	UAuthenticator* authenticator;
	UAuth* auth;//for persistent auth, used to read persistent stored data for automatic authentication

	//testing variables//
	bool enableTesting = true;
	TArray<FCoin_BE> testCoins;
	TArray<FNFT_Master_BE> testNFTs;
	//testing variables//

public:
	FUserDetails getUserDetails();

	// Sets default values for this actor's properties
	ASequenceBackendManager();
	//destructor for cleaning up old refs
	~ASequenceBackendManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
//SYNC FUNCTIONAL CALLS// [THESE ARE BLOCKING CALLS AND WILL RETURN DATA IMMEDIATELY]
	/*
	* Used to send data to clipboard for ease of use!
	*/
	UFUNCTION(BlueprintCallable, CATEGORY="FUNCTION")
		void CopyToClipboard(FString data);

	/*
	* Used to get data from clipboard for ease of use
	*/
	UFUNCTION(BlueprintCallable, CATEGORY = "FUNCTION")
		FString GetFromClipboard();

	UFUNCTION(BlueprintCallable, CATEGORY = "FUNCTION")
		FString GetTransactionHash(FTransaction_FE Transaction);

	//this needs to be changed such that our creds stay within the C++ realm not the blueprint realm for ease of use
	//Used to get the stored auth credentials encrypt them and send them up
	//to be stored in a game save object
	UFUNCTION(BlueprintCallable, CATEGORY = "SecureStorage")
		FSecureKey GetSecureStorableAuth();

	UFUNCTION(BlueprintCallable, CATEGORY = "Login")
		FString GetLoginURL(const ESocialSigninType& Type);

	UFUNCTION(BlueprintCallable, CATEGORY = "Login")
		FString GetRedirectURL();

	UFUNCTION(BlueprintCallable, CATEGORY = "Login")
		void SocialLogin(const FString& IDTokenIn);

	UFUNCTION(BlueprintCallable, CATEGORY = "Login")
		void EmailLogin(const FString& EmailIn);

	UFUNCTION(BlueprintCallable, CATEGORY = "Login")
		void EmailCode(const FString& CodeIn);

//SYNC FUNCTIONAL CALLS// [THESE ARE BLOCKING CALLS AND WILL RETURN DATA IMMEDIATELY]

//ASYNC FUNCTIONAL CALLS// [THESE ARE NON BLOCKING CALLS AND WILL USE A MATCHING UPDATE...FUNC TO RETURN DATA]

	//these are the 2 receive functions we have
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, CATEGORY = "Receive")
		void ReceiveCoin(FCoin_BE coin);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, CATEGORY = "Receive")
		void ReceiveNFT(FNFT_Master_BE nft);
	/*
	* Used to initiate a stored authentication call from the frontend
	*/
	UFUNCTION(BlueprintCallable, CATEGORY = "Authentication")
		void InitAuthentication(FSecureKey storedAuthData);

	/*
	* Used to let the frontend know if authentication succeeded or not
	* in an async. manner
	* 
	* if authenticated is false then we failed to auth the user or timed out
	* if authenticated is true we successfully authenticated the user and we need to signal the ui that
	* we are ready for next steps
	* 
	*/
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, CATEGORY = "Authentication")
		void UpdateAuthentication(bool authenticated);

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
		void UpdateTxn(FTxnCallback_BE txnCallback);

	UFUNCTION(BlueprintCallable, CATEGORY = "Get_Updated_Data")
		void InitGetUpdatedCoinData(TArray<FID_BE> CoinsToUpdate);

	UFUNCTION(BlueprintCallable, CATEGORY = "Get_Updated_Data")
		void InitGetUpdateTokenData(TArray<FID_BE> TokensToUpdate);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, CATEGORY = "Get_Updated_Data")
		void UpdateCoinData(const TArray<FItemPrice_BE> &updatedCoinValues);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, CATEGORY = "Get_Updated_Data")
		void UpdateTokenData(const TArray<FItemPrice_BE> &updatedTokenValues);

//ASYNC FUNCTIONAL CALLS// [THESE ARE NON BLOCKING CALLS AND WILL USE A MATCHING UPDATE...FUNC TO RETURN DATA]

private:
	//testing functions//

	UFUNCTION()
		void RandomReceive();

	//testing functions//
};
