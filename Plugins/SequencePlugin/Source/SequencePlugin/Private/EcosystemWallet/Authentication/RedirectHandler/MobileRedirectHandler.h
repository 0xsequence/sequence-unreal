#pragma once

#include "CoreMinimal.h"
#include "IRedirectHandler.h"

class SEQUENCEPLUGIN_API FMobileRedirectHandler : public IRedirectHandler
{
public:
	static TSharedPtr<TSuccessCallback<FString>> StrongOnSuccess;
	static TSharedPtr<FFailureCallback> StrongOnFailure;

	static TWeakPtr<TSuccessCallback<FString>> CurrentOnSuccess;
	static TWeakPtr<FFailureCallback> CurrentOnFailure;

	static void HandleResponse(const FString& Url);
	static void HandleError(const FString& Error);

	static void LogMessage(const FString& Message);
	
protected:
	virtual void WaitForResponseImpl(const FString& FullUrl, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure) override;

private:
	static TMap<FString, FString> ConvertUrlToQueryMap(const FString& Url);
};
