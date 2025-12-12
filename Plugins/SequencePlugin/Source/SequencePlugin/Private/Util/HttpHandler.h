#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Util/Async.h"
#include "HttpHandler.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API UHttpHandler : public UObject
{
	GENERATED_BODY()

public:
	void SetRequestUrl(const FString& Url);
	void SendPostRequest(const FString& Endpoint, const FString& Payload, TSuccessCallback<FString> OnSuccess, TFunction<void(FString)> OnFailure) const;

private:
	FString RequestUrl;
};
