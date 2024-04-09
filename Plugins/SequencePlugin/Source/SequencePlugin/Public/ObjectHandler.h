// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Http.h"
#include "Engine/Texture2D.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "UObject/NoExportTypes.h"
#include "Modules/ModuleManager.h"
#include "Syncer.h"
#include "ObjectHandler.generated.h"

DECLARE_DELEGATE(FOnDoneImageProcessingSignature);

class UIndexer;


USTRUCT()
struct FRawData //used to store raw data in our runtime cache!
{
	GENERATED_USTRUCT_BODY()
public:
	TArray<uint8> RawData;
};

/**
 * 
 */
UCLASS()
class UObjectHandler : public UObject
{
	GENERATED_BODY()
public:
	FOnDoneImageProcessingSignature FOnDoneImageProcessingDelegate;
	TMap<FString, UTexture2D*> GetProcessedImages();
private:
	//note 2GB is the limit because of int32 size limits!
	const int32 MaxCacheSize = 256 * 1024 * 1024;//max size in bytes! 256 MB is what i have this set to for now!
	bool bUseCustomFormat = false;
	EImageFormat CustomFormat = EImageFormat::GrayscaleJPEG;
	int32 CurrentCacheSize = 0;//in bytes we want this as accurate as possible!
	TMap<FString, FRawData> Cache;//a Map of URL's and rawData
	//need a list of maps!
	TMap<FString, UTexture2D*> StoredResponses;

	bool UseRawCache = false; // enables / disables the use of raw data caching!

	//this is the number of requests that are actively processing
	USyncer* Syncer;

	/*
	* Attempts to filter out bad urls to save on processing time
	* Checks for http:// & https:// and length greater than 7
	*/
	TArray<FString> FilterURLs(TArray<FString> Urls);

	/*
	* Checks the cache to see if we already have the data needed in it
	* if so fetch it from the cache.
	* @return whether we had a cache hit or a cache miss
	*/
	bool CheckRawCache(FString URL,TArray<uint8>* RawData);

	/*
	* adds raw data to cache if the it can fit in the cache,
	* if it can't we will remove data at random until we can fit this new
	* data in
	* @param URL the url we got the data from
	* @param raw_data the raw byte data from the given url
	*/
	void AddToCache(FString URL, TArray<uint8> RawData);

	/*
	* Simple check to see if the cache can accomadate additional bytes
	* @param byte_count_to_add the amount of byte data we would like to add to the cache
	* @return true if we can add it else false
	*/
	bool CanAddToCache(int32 ByteCountToAdd);

public:

	/*
	* Used to setup the Raw handler for web requests
	*/
	void Setup(bool RawCacheEnabled);

	void SetupCustomFormat(bool RawCacheEnabled, EImageFormat Format);

	//used to clear the contents of the raw cache!
	void ClearRawCache();

private:
	UFUNCTION()
	void OnDone();

	void StoreImageData(UTexture2D *Image, FString URL);

	//Here we process the data as uint8[] (For images)
	void HandleRequestRaw(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	/*
	* Base implementation of async raw request (IE getting raw uint8 bytes!) calls the associated
	* raw handler set for this object handler
	* @return true if the request started successfully else false
	*/
	bool RequestRawBase(FString URL);

public:

	/*
	* Basic raw request
	* @upon request completetion a delegate OnDone will fire that will contain the needed data!
	* in the event an error occurs OnDone will fire with nothing in it
	*/
	void RequestImage(FString URL);//this is specifically there for getting raw data!

	/*
	* Basic raw multi request
	* @upon request completetion a delegate OnDone will fire that will contain the needed data!
	* in the event an error occurs OnDone will fire with nothing in it
	*/
	void RequestImages(TArray<FString> URLs);//this is specifically there for getting raw data!

private:
	//helper functions for handling requests data! specifically image data!
	/*
	* Used to build a texture2d from raw byte data, we include the URL
	* for format identification purposes
	* @return the generated texture2d
	*/
	UTexture2D* BuildImgData(TArray<uint8> ImgData,FString URL);

	UTexture2D* TryBuildImage(TArray<uint8> ImgData, EImageFormat Format);

	/*
	* Used to get the img format out of a url
	*/
	EImageFormat GetImgFormat(FString URL);
};