// Fill out your copyright notice in the Description page of Project Settings.


#include "RequestHandler.h"

#include "Async.h"
#include "Http.h"
#include "HttpManager.h"
#include "Types/BinaryData.h"

URequestHandler* URequestHandler::PrepareRequest()
{
	Request = FHttpModule::Get().CreateRequest();
	return this;
}

void URequestHandler::SetUrl(const FString Url) const
{
	Request->SetURL(Url);
}

void URequestHandler::SetVerb(FString Verb) const
{
	Request->SetVerb(Verb);
}

void URequestHandler::AddHeader(const FString Name, const FString Value) const
{
	Request->AppendToHeader(Name, Value);
}

void URequestHandler::SetContentAsString(const FString Content) const
{
	Request->SetContentAsString(Content);
}

URequestHandler* URequestHandler::WithUrl(const FString Url)
{
	SetUrl(Url);
	return this;
}

URequestHandler* URequestHandler::WithVerb(const FString Verb)
{
	SetVerb(Verb);
	return this;
}

URequestHandler* URequestHandler::WithHeader(const FString Name, const FString Value)
{
	AddHeader(Name, Value);
	return this;
}

URequestHandler* URequestHandler::WithContentAsString(const FString Content)
{
	UE_LOG(LogTemp, Display, TEXT("Request content: %s"), *Content);
	SetContentAsString(Content);
	return this;
}

FHttpRequestCompleteDelegate& URequestHandler::Process() const
{
	Request->ProcessRequest();
	return Request->OnProcessRequestComplete();
}



void URequestHandler::ProcessAndThen(TFunction<void (FString)> OnSuccess, FFailureCallback OnFailure)
{
	Process().BindLambda([OnSuccess, OnFailure](FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		UE_LOG(LogTemp, Display, TEXT("Request URL: %s"), *Req->GetURL());
		
		auto content = Req->GetContent();
		FString str = "";
		for(auto i : content)
		{
			str += UTF8ToString(FUnsizedData{&i, 1});
		}
		UE_LOG(LogTemp, Display, TEXT("Request Content: %s"), *str);

		auto headers = Req->GetAllHeaders();
		FString headers_str = "";
		for(auto header : headers) { headers_str += "\n" + header; }
		UE_LOG(LogTemp, Display, TEXT("Request Headers: %s"), *headers_str);
		
		if(bWasSuccessful)
		{
			auto Content = Response->GetContentAsString();
			OnSuccess(Content);
		}
		else
		{
			if(!Response.IsValid())
			{
				return OnFailure(SequenceError(RequestFail, "The Request is invalid!"));
			}
			
			OnFailure(SequenceError(RequestFail, "Request failed: " + Response->GetContentAsString()));
		}
	});
}




