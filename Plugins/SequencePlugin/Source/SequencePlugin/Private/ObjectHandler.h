// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Http.h"
#include "Engine/Texture2D.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "UObject/NoExportTypes.h"
#include "ObjectHandler.generated.h"

class UIndexer;

/**
 * 
 */
UCLASS()
class UObjectHandler : public UObject
{
	GENERATED_BODY()
	
private:
	//this index we will be inserting into
	UObject* main_this_ref = NULL;
	TMap<FString, int32> insertion_indices;//we can associate URL's with insertion indices!

	typedef void(UObjectHandler::*string_handler)(FString response,FString URL);//pass in data it absolutely needs to seperate!
	typedef void(UObjectHandler::*raw_handler)(TArray<uint8> response,FString URL);

	//used to process string based responses!
	string_handler main_string_handler;
	bool string_handler_ready = false;

	//used to process raw uint8 based responses!
	raw_handler main_raw_handler;
	bool raw_handler_ready = false;

	/*
	* Adds a KVP (URL,i_index) to insertion indices
	*/
	void add_to_insertion_indices(FString URL,int32 i_index);

	/*
	* Removes a Key from the insertion indices if it exists!
	*/
	void remove_from_insertion_indices(FString URL);

public:
	/*
	* Used to setup the String handler for web requests
	*/
	void setup_string_handler(string_handler handler_in, UObject * this_ref);

	/*
	* Used to setup the Raw handler for web requests
	*/
	void setup_raw_handler(raw_handler handler_in, UObject * this_ref);

	//here we process the data as json (For json responses)
	void handle_request_string(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	//Here we process the data as uint8[] (For images)
	void handle_request_raw(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

private:

	/*
	* Base implementation of async json request, calls the associated string handler set
	* for this object handler!
	* @return true if the request started successfully else false
	*/
	bool request_json_base(FString URL, FString json_args);

	/*
	* Base implementation of async raw request (IE getting raw uint8 bytes!) calls the associated
	* raw handler set for this object handler
	* @return true if the request started successfully else false
	*/
	bool request_raw_base(FString URL);

public:

	/*
	* Basic json request
	* @return true if req. started else false
	*/
	bool request_json(FString URL, FString json_args);

	/*
	* Index json request, initiates like a base request json BUT
	* when complete there will be an index provided via the TMap indices that is associated with
	* the requests URL this index can be used in a handler to index into a TArray in the Callers stores!
	* @return whether the txn started or not
	*/
	bool request_json_indexed(FString URL, FString json_args, int32 index);

	/*
	* Basic raw request
	* @return true if req. started else false
	*/
	bool request_raw(FString URL);//this is specifically there for getting raw data!

	/*
	* Index raw request, initiates like a base request raw BUT
	* when complete there will be an index provided via the TMap indices that is associated with
	* the requests URL this index can be used in a handler to index into a TArray in the Callers stores!
	* @return whether the txn started or not
	*/
	bool request_raw_indexed(FString URL, int32 index);

public:

//RAW HANDLERS//
	/*
	* Custom handler for handling received image data!
	* this will call add_img in Sequence_Backend_Manager to add
	* the newly built image data from the request
	*/
	void img_handler(TArray<uint8> response,FString URL);
//RAW HANDLERS//

private:
	//helper functions for handling requests data! specifically image data!
	UTexture2D* build_img_data(TArray<uint8> img_data,FString URL);
	EImageFormat get_img_format(FString URL);
};