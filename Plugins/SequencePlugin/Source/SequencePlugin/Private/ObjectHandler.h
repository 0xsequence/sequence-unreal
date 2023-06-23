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
	int32 insertion_index = -1;//this will ensure we throw an error if we don't setup first!
	typedef void(UObjectHandler::*string_handler)(FString response,FString URL);//pass in data it absolutely needs to seperate!
	typedef void(UObjectHandler::*raw_handler)(TArray<uint8> response,FString URL);

	//used to process string based responses!
	string_handler main_string_handler;
	bool string_handler_ready = false;

	//used to process raw uint8 based responses!
	raw_handler main_raw_handler;
	bool raw_handler_ready = false;

public:
	void setup_string_handler(int32 i_index, string_handler handler_in, UObject * this_ref);
	void setup_raw_handler(int32 i_index, raw_handler handler_in, UObject * this_ref);

	void handle_request(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	//here we process the data as json (For json responses)
	void handle_request_string(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	//Here we process the data as uint8[] (For images)
	void handle_request_raw(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	bool request_json(FString URL, FString json_args);

	bool request_raw(FString URL);//this is specifically there for getting raw data!

public:
	void img_handler(TArray<uint8> response,FString URL);
	UTexture2D* build_img_data(TArray<uint8> img_data,FString URL);
	EImageFormat get_img_format(FString URL);
};