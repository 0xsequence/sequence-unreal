// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Http.h"
#include "ObjectHandler.h"
#include "Backend.h"
#include "BE_Structs.h"
#include "Misc/AES.h"
#include "Auth.h"
#include "Sequence_Backend_Manager.generated.h"

class UIndexer;

UCLASS()
class SEQUENCEPLUGIN_API ASequence_Backend_Manager : public AActor, public IBackend
{
	GENERATED_BODY()

private:
	FString glb_PublicAddress = "0x8e3E38fe7367dd3b52D1e281E4e8400447C8d8B9";//this is the signed in public addr
	int64 glb_ChainID = 137; //this is the chain we are currently using

	FString prvt_key; // private key for signin
	FString pblc_key; // public key for signin
	TArray<FString> hex_data;//this is our LUT of hexidecimal data!

	UIndexer* indexer;//indexer ref!

	UObjectHandler* request_handler;//going to be reworked into an image handler

	UAuth* auth;//for auth handling
	
	//for right now we use these variables to bootstrap signin
	bool ready = false;
	FString recv_block_num;
	int32 recv_id;
	FString recv_block_hsh;
	FString user_email;
	//end of signin variables

public:
	// Sets default values for this actor's properties
	ASequence_Backend_Manager();

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
		void Copy_To_Clipboard(FString data);
	/*
	* Used to get data from clipboard for ease of use
	*/
	UFUNCTION(BlueprintCallable, CATEGORY = "FUNCTION")
		FString Get_From_Clipboard();

	UFUNCTION(BlueprintCallable, CATEGORY = "FUNCTION")
		FString get_transaction_hash();

	//Used to get the stored auth credentials encrypt them and send them up
	//to be stored in a game save object
	UFUNCTION(BlueprintCallable, CATEGORY = "SecureStorage")
		FSecureKey getSecureStorableAuth();

//SYNC FUNCTIONAL CALLS// [THESE ARE BLOCKING CALLS AND WILL RETURN DATA IMMEDIATELY]

//ASYNC FUNCTIONAL CALLS// [THESE ARE NON BLOCKING CALLS AND WILL USE A MATCHING UPDATE...FUNC TO RETURN DATA]

	/*
	* Used to initiate a stored authentication call from the frontend
	*/
	UFUNCTION(BlueprintCallable, CATEGORY = "Authentication")
		void init_authentication(FSecureKey storedAuthData);

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
		void update_authentication(bool authenticated);

	/*
	* Used to init. a call to fetch all system data needed by frontend in an async manner
	* Once system data struct is built we send it up with update_system_data(FSystemData_BE)
	*/
	UFUNCTION(BlueprintCallable, CATEGORY = "SystemData")
		void init_system_data();

	/*
	* Used to update the frontend with the supplied system data
	* in an async manner
	*/
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, CATEGORY = "SystemData")
		void update_system_data(const FSystemData_BE &system_data);

	/*
	* Used to initate the signin process from the frontend
	* This call will make 2 calls
	* 1) to generate an oob code and send that back to the front end
	* 2) a call to authenticate user credentials in relation to the oob code that was sent out
	* If authentication succeeds we send update_authentication(true)
	* If authentication fails (times out / error) we send update_authentication(false)
	*/
	UFUNCTION(BlueprintCallable, CATEGORY="Signin")
		void init_signin(FString email);

	/*
	* Used to tell the frontend that the signin process has been initiated and the code here is present!
	*/
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, CATEGORY = "Signin")
		void update_signin(const FString &oob_code);

	/*
	* Used to initalize a send transaction!
	*/
	UFUNCTION(BlueprintCallable, CATEGORY = "Send_Txn")
		void init_coin_send_txn(FCoin_Send_Txn_BE coin_txn);

	UFUNCTION(BlueprintCallable, CATEGORY = "Send_Txn")
		void init_nft_send_txn(FNFT_Send_Txn_BE nft_txn);

	/*
	* Used to let the frontend know if a txn went through or not!
	*/
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, CATEGORY = "Send_Txn")
		void update_txn(FTxnCallback_BE txn_callback);

	UFUNCTION(BlueprintCallable, CATEGORY = "Get_Updated_Data")
		void init_get_updated_coin_data(TArray<FID_BE> coinsToUpdate);

	UFUNCTION(BlueprintCallable, CATEGORY = "Get_Updated_Data")
		void init_get_updated_token_data(TArray<FID_BE> tokensToUpdate);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, CATEGORY = "Get_Updated_Data")
		void updateCoinData(const TArray<FItemPrice_BE> &updatedCoinValues);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, CATEGORY = "Get_Updated_Data")
		void updateTokenData(const TArray<FItemPrice_BE> &updatedTokenValues);

//ASYNC FUNCTIONAL CALLS// [THESE ARE NON BLOCKING CALLS AND WILL USE A MATCHING UPDATE...FUNC TO RETURN DATA]

private:
	//Private handlers
	void signin_handler(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void get_blk_handler(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void get_hsh_handler(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	//Private handlers

	/*
	* Used for sending out ASYNC requests used to handle signin
	* NOTE: Looking at moving this code to a dedicated networking class for reuse
	* @Param URL (the full url to send the json payload to)
	* @Param json (the json payload in FString form) If invalid we will get a response but it'll be an error mesg from the server in json format
	* @param Handler (The ASYNC handler that will process the payload once we have received it)
	*/
	void post_json_request(FString url, FString json, void (ASequence_Backend_Manager::* handler)(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful));
	
	/*
	* Used to request block information from
	* the sequence app, 
	* @Return the block data we want from the latest block on the chain
	*/
	FString create_blk_req();

	/*
	* Used to get the block hash data based on the block number and givin id
	* Fetches this data from the sequence app
	* @Return the block hash we want!
	*/
	FString create_hsh_req(FString blk_num, int32 id);

	/*
	* Sets up the signin request body
	* @Return the signin req in json format in an FString
	*/
	FString create_req_body(FString email);

	/*
	* Sets up the intent for Signin
	* @Return the Intent in Json format in an FString
	*/
	FString create_intent(FString email);

	/*
	* Generates a random wallet ID used to bootstrap signin
	* @Return a Random wallet ID
	*/
	FString setup_random_wallet();//returns the public key!

	//These functions are used to generate the URL's need to interact with
	//the various aspects of the sequence app
	FString get_main_url();
	FString get_continue_url();
	FString get_signin_url();
};
