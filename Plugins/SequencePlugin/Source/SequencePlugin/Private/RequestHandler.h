#pragma once

#include "CoreMinimal.h"
#include "Util/Async.h"
#include "Interfaces/IHttpRequest.h"
#include "Engine/Texture2D.h"
#include "UObject/Object.h"
#include "ResponseSignatureValidator.h"
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
	void ProcessAndThen(UResponseSignatureValidator& Validator, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure, bool bUseValidator = true) const;
	void ProcessAndThen(const TSuccessCallback<FHttpResponsePtr>& OnSuccess, const FFailureCallback& OnFailure) const;
	void ProcessAndThen(const TSuccessCallback<UTexture2D*>& OnSuccess, const FFailureCallback OnFailure) const;

};
