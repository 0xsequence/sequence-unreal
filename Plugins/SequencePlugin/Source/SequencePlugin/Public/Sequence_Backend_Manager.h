// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Indexer.h"
#include "Sequence_Backend_Manager.generated.h"
UCLASS()
class SEQUENCEPLUGIN_API ASequence_Backend_Manager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASequence_Backend_Manager();

	/*
	* Used to setup the backend manager for usage
	* retuns FString ("Complete Message")
	*/
	UFUNCTION(BlueprintCallable, CATEGORY="Setup")
		FString Setup();

	/*
	* Used to test an indivual private key, to see how it's generated public key compares to a CORRECT
	* pre computed and and how the generated address from the generated public key compares to a precomputed
	* CORRECT address
	* returns true IFF gen_public_key matches the hard_public_key &&
	* gen_address matches hard_address
	*/
	bool test_address_gen(FString prvt_k, FString hrd_pblc_k, FString hrd_addr);

	/*
	* Used to test the public key generation and address generation in accordance with
	* the test chain!
	*/
	UFUNCTION(BlueprintCallable, CATEGORY="TESTING")
		FString Testing_Address_Generation();

	UFUNCTION(BlueprintCallable, CATEGORY="TESTING")
		TArray<UTexture2D*> Testing_Indexer();

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

	/*
	* Used to initiate a passwordless signin!
	* @param FString email (email in)
	* @return the Oob code to be displayed for the user to enter on the site login!
	* NOTE: for the code we are expecting 6 digits!
	*/
	UFUNCTION(BlueprintCallable, CATEGORY = "FUNCTION")
		FString Signin(FString email);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void signin_handler(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void get_blk_handler(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void get_hsh_handler(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	bool ready = false;
	FString recv_block_num;
	int32 recv_id;
	FString recv_block_hsh;
	FString user_email;
	void post_json_request(FString url, FString json, void (ASequence_Backend_Manager::* handler)(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful));
	
	FString create_blk_req();
	FString create_hsh_req(FString blk_num, int32 id);

	FString prvt_key;
	FString pblc_key;

	TArray<FString> hex_data;//this may be useless now

	FString create_req_body(FString email);

	FString create_intent(FString email);

	FString setup_random_wallet();//returns the public key!

	FString get_main_url();
	FString get_continue_url();
	FString get_signin_url();

};
