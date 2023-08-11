// Fill out your copyright notice in the Description page of Project Settings.


#include "SequenceData.h"


FString USequenceData::getURL(FString endpoint)
{
	FString retUrl = sequenceURL;
	retUrl.Append(endpoint);
	return retUrl;
}

/*
	Here we construct a post request and parse out a response if valid.
*/
void USequenceData::HTTPPost(FString endpoint, FString args, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure)
{
	//Now we create the post request
	TSharedRef<IHttpRequest> http_post_req = FHttpModule::Get().CreateRequest();

	http_post_req->SetVerb("POST");
	http_post_req->SetHeader("Content-Type", "application/json");//2 differing headers for the request
	http_post_req->SetHeader("Accept", "application/json");
	FString url = this->getURL(endpoint);
	http_post_req->SetURL(this->getURL(endpoint));
	http_post_req->SetContentAsString(args);//args will need to be a json object converted to a string

	http_post_req->OnProcessRequestComplete().BindLambda([=](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (bWasSuccessful)
			{
				FString Content = Request->GetResponse()->GetContentAsString();
				UE_LOG(LogTemp, Error, TEXT("Response: [%s]"), *Content);
				OnSuccess(Content);
			}
			else
			{
				OnFailure(SequenceError(RequestFail, "Request failed: " + Request->GetResponse()->GetContentAsString()));
			}
		});
	http_post_req->ProcessRequest();
}

void HTTPGet(FString endpoint, FString args, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure)
{
	//Now we create the Get request
	TSharedRef<IHttpRequest> http_post_req = FHttpModule::Get().CreateRequest();

	http_post_req->SetVerb("GET");
	//this is more or less custom tuned for the QR fetcher still waiting on the setup of that though!
	http_post_req->SetHeader("Content-Type", "application/json");//2 differing headers for the request
	http_post_req->SetHeader("Accept", "application/json");
	http_post_req->SetURL(endpoint);

	http_post_req->OnProcessRequestComplete().BindLambda([=](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (bWasSuccessful)
			{
				FString Content = Request->GetResponse()->GetContentAsString();
				UE_LOG(LogTemp, Error, TEXT("Response: [%s]"), *Content);
				OnSuccess(Content);
			}
			else
			{
				OnFailure(SequenceError(RequestFail, "Request failed: " + Request->GetResponse()->GetContentAsString()));
			}
		});
	http_post_req->ProcessRequest();
}

TArray<FContact_BE> USequenceData::buildFriendListFromJson(FString json)
{
	TArray<FContact_BE> friendList;
	TSharedPtr<FJsonObject> jsonObj;

	if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(json), jsonObj))
	{
		const TArray<TSharedPtr<FJsonValue>>* storedFriends;
		if (jsonObj.Get()->TryGetArrayField("friends", storedFriends))
		{
			for (TSharedPtr<FJsonValue> friendData : *storedFriends)
			{
				const TSharedPtr<FJsonObject>* fJsonObj;
				if (friendData.Get()->TryGetObject(fJsonObj))//need it as an object
				{
					FContact_BE newFriend;
					newFriend.Public_Address = fJsonObj->Get()->GetStringField("userAddress");
					newFriend.Nickname = fJsonObj->Get()->GetStringField("nickname");
					friendList.Add(newFriend);
				}
			}
		}
	}
	else
	{//failure
		UE_LOG(LogTemp, Error, TEXT("Failed to convert String: %s to Json object"), *json);
	}
	return friendList;
}

/*
* Gets the friend data from the given username!
* This function appears to require some form of authentication (perhaps all of the sequence api does)
*/
void USequenceData::getFriends(FString username, TSuccessCallback<TArray<FContact_BE>> OnSuccess, FFailureCallback OnFailure)
{
	FString json_arg = "{}";

	HTTPPost("friendList", json_arg, [=](FString Content)
		{
			OnSuccess(buildFriendListFromJson(Content));
		}, OnFailure);
}

TArray<FItemPrice_BE> USequenceData::buildItemUpdateListFromJson(FString json)
{
	TSharedPtr<FJsonObject> jsonObj;
	FUpdatedPriceReturn updatedPrices;

	if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(json), jsonObj))
	{
		if (FJsonObjectConverter::JsonObjectToUStruct<FUpdatedPriceReturn>(jsonObj.ToSharedRef(), &updatedPrices))
		{
			return updatedPrices.tokenPrices;
		}
	}
	else
	{//failure
		UE_LOG(LogTemp, Error, TEXT("Failed to convert String: %s to Json object"), *json);
	}
	TArray<FItemPrice_BE> updatedItems;
	return updatedItems;
}

void USequenceData::getUpdatedItemPrice(FID_BE itemToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure)
{
	TArray<FID_BE> items;
	items.Add(itemToUpdate);
	getUpdatedItemPrices(items, OnSuccess, OnFailure);
}

void USequenceData::getUpdatedItemPrices(TArray<FID_BE> itemsToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure)
{
	FString args = "{\"tokens\":";
	FString jsonObjString = "";
	TArray<FString> parsedItems;
	for (FID_BE item : itemsToUpdate)
	{
		if (FJsonObjectConverter::UStructToJsonObjectString<FID_BE>(item,jsonObjString))
			parsedItems.Add(jsonObjString);
	}
	args += UIndexerSupport::stringListToSimpleString(parsedItems);
	args += "}";

	HTTPPost("getCoinPrices", args, [=](FString Content)
		{
			OnSuccess(buildItemUpdateListFromJson(Content));
		}, OnFailure);
}

void USequenceData::getQR(FString publicAddress, int32 size, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure)
{//still need authentication for this to work!
	int32 lclSize = FMath::Max(size, 64);//ensures a nice valid size
	FString args = "{\"publicAddress\":\""+publicAddress+"\"}";
	FString urlSize = "/";
	urlSize.AppendInt(size);
	HTTPPost(sequenceURL_QR+urlSize, args, [=](FString Content)
		{
			OnSuccess(Content);
		}, OnFailure);
}