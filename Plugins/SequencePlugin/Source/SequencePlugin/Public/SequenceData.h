// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Async.h"
#include "BE_Structs.h"
#include "JsonBuilder.h"
#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "Http.h"
#include "IndexerSupport.h"
#include "HttpManager.h"
#include "UObject/NoExportTypes.h"
#include "SequenceData.generated.h"

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API USequenceData : public UObject
{
	GENERATED_BODY()
	
private:
	//for some reason unreal parses this funny but the calls still go through?
	const FString sequenceURL = "https://api.sequence.app/";

	const FString sequenceURL_QR = "https://api.sequence.app/qr/BASE64-URL-ENCODED-STRING";

	FString getURL(FString endpoint);

	void HTTPPost(FString endpoint, FString args, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);

	void HTTPGet(FString endpoint, FString args, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);

	TArray<FContact_BE> buildFriendListFromJson(FString json);

	TArray<FItemPrice_BE> buildItemUpdateListFromJson(FString json);
public:
	void getFriends(FString publicAddress, TSuccessCallback<TArray<FContact_BE>> OnSuccess, FFailureCallback OnFailure);

	void getUpdatedItemPrice(FID_BE itemToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure);

	void getUpdatedItemPrices(TArray<FID_BE> itemsToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure);

	void getQR(FString publicAddress,int32 size,TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);
};

USTRUCT(BlueprintType)
struct FUpdatedPriceReturn
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FItemPrice_BE> tokenPrices;
};