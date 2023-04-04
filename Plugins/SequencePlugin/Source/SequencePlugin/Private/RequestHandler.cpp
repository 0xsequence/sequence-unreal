// Fill out your copyright notice in the Description page of Project Settings.


#include "RequestHandler.h"
#include "Http.h"
#include "HttpManager.h"

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

void URequestHandler::SetHeader(const FString Name, const FString Value) const
{
	Request->SetHeader(Name, Value);
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
	SetHeader(Name, Value);
	return this;
}

URequestHandler* URequestHandler::WithContentAsString(const FString Content)
{
	UE_LOG(LogTemp, Display, TEXT("Request content: %s"), *Content);
	SetContentAsString(Content);
	return this;
}

// This is a hacky way to make it synchronous
// TODO: Find a better method
FHttpResponsePtr URequestHandler::Process()
{
	Request->ProcessRequest();

	double LastTime = FPlatformTime::Seconds();
	while(EHttpRequestStatus::Processing == Request->GetStatus())
	{
		const double AppTime = FPlatformTime::Seconds();
		FHttpModule::Get().GetHttpManager().Tick(AppTime - LastTime);
		LastTime = AppTime;
		FPlatformProcess::Sleep(0.5f);
	}

	return Request->GetResponse();
}




