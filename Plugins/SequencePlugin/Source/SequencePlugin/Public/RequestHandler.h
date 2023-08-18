// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Async.h"
#include "Interfaces/IHttpRequest.h"
#include "UObject/Object.h"
#include "RequestHandler.generated.h"

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API URequestHandler : public UObject
{
	GENERATED_BODY()
	FHttpRequestPtr Request;
	
public:
	URequestHandler* PrepareRequest();
	
	// Setters
	void SetUrl(FString Url) const;
	void SetVerb(FString Verb) const;
	void AddHeader(FString Name, FString Value) const;
	void SetContentAsString(FString Content) const;

	// Builder Pattern
	URequestHandler* WithUrl(FString Url);
	URequestHandler* WithVerb(FString Verb);
	URequestHandler* WithHeader(FString Name, FString Value);
	URequestHandler* WithContentAsString(FString Content);

	// Process
	FHttpRequestCompleteDelegate& Process() const;
	void ProcessAndThen(TFunction<void (FString)> OnSuccess, FFailureCallback OnFailure);
};
