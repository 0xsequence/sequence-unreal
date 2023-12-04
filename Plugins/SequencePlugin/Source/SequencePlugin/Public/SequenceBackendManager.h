// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Http.h"
#include "ObjectHandler.h"
#include "Indexer/Backend.h"
#include "Util/Structs/BE_Structs.h"
#include "Misc/AES.h"
#include "Auth.h"
#include "Sequence/SequenceAPI.h"
#include "Authenticator.h"
#include "SequenceBackendManager.generated.h"

class UIndexer;

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

UCLASS()
class SEQUENCEPLUGIN_API ASequenceBackendManager : public AActor, public IBackend
{
	GENERATED_BODY()

private:
	FString publicAddress = "0x8e3E38fe7367dd3b52D1e281E4e8400447C8d8B9";//this is the signed in public addr
	int64 chainID = 137; //this is the chain we are currently using

	FString privateKey; // private key for signin
	FString publicKey; // public key for signin
	TArray<FString> hexDataList;//this is our LUT of hexidecimal data!
	SequenceAPI::FSequenceWallet* sequenceWallet = nullptr;
	UIndexer* Indexer;//indexer ref!
	UAuthenticator* authenticator;
	UObjectHandler* requestHandler;//going to be reworked into an image handler
	//https://accounts.google.com/o/oauth2/auth?response_type=id_token&client_id=970987756660-35a6tc48hvi8cev9cnknp0iugv9poa23.apps.googleusercontent.com&redirect_uri=https://3d41-142-115-54-118.ngrok-free.app/&scope=openid+profile+email&state=604063DB47CAAC7E7547A789E7BC3244&nonce=81C518BB46601888C649D9BDF58A2DAA
	UAuth* auth;//for auth handling
	
	//for right now we use these variables to bootstrap signin TBD (this may get moved an AuthManager.cpp / .h setup instead for a cleaner setup
	bool ready = false;
	FString receiveBlockNumber;
	int32 receiveID;
	FString receiveBlockHash;
	FString userEmail;
	int32 accountID;
	FString emailService;
	FString username;
	//end of signin variables

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

	// Called every frame
	virtual void Tick(float DeltaTime) override;

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

	//Used to get the stored auth credentials encrypt them and send them up
	//to be stored in a game save object
	UFUNCTION(BlueprintCallable, CATEGORY = "SecureStorage")
		FSecureKey GetSecureStorableAuth();

	UFUNCTION(BlueprintCallable, CATEGORY = "Login")
		FString GetLoginURL();//this will eventually have an enum for our login type!

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
	* if authenticated is false the we failed to auth the user or timed out
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
	* Used to initate the signin process from the frontend
	* This call will make 2 calls
	* 1) to generate an oob code and send that back to the front end
	* 2) a call to authenticate user credentials in relation to the oob code that was sent out
	* If authentication succeeds we send update_authentication(true)
	* If authentication fails (times out / error) we send update_authentication(false)
	*/
	UFUNCTION(BlueprintCallable, CATEGORY="Signin")
		void InitSignin(FString Email);

	/*
	* Used to tell the frontend that the signin process has been initiated and the code here is present!
	*/
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, CATEGORY = "Signin")
		void UpdateSignin(const FString &oobCode);

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
	//Private handlers
	void SigninHandler(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void GetBlkHandler(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void GetHashHandler(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	//Private handlers

	/*
	* Used for sending out ASYNC requests used to handle signin
	* NOTE: Looking at moving this code to a dedicated networking class for reuse
	* @Param URL (the full url to send the json payload to)
	* @Param json (the json payload in FString form) If invalid we will get a response but it'll be an error mesg from the server in json format
	* @param Handler (The ASYNC handler that will process the payload once we have received it)
	*/
	void PostJsonRequest(FString url, FString json, void (ASequenceBackendManager::* handler)(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful));
	
	/*
	* Used to request block information from
	* the sequence app, 
	* @Return the block data we want from the latest block on the chain
	*/
	FString CreateBlkRequest();

	/*
	* Used to get the block hash data based on the block number and givin id
	* Fetches this data from the sequence app
	* @Return the block hash we want!
	*/
	FString CreateHashRequest(FString blockNumber, int32 id);

	/*
	* Sets up the signin request body
	* @Return the signin req in json format in an FString
	*/
	FString CreateRequestBody(FString Email);

	/*
	* Sets up the intent for Signin
	* @Return the Intent in Json format in an FString
	*/
	FString CreateIntent(FString Email);

	/*
	* Generates a random wallet ID used to bootstrap signin
	* @Return a Random wallet ID
	*/
	FString SetupRandomWallet();//returns the public key!

	//These functions are used to generate the URL's need to interact with
	//the various aspects of the sequence app
	FString GetMainURL();
	FString GetContinueURL();
	FString GetSigninURL();

	//testing functions//

	UFUNCTION()
		void RandomReceive();

	//testing functions//
};
