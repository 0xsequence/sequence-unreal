#pragma once

#include "CoreMinimal.h"
#include "Models/FeeOptionsArgs.h"
#include "Models/FeeOptionsResponse.h"
#include "Models/GetMetaTxnReceiptResponse.h"
#include "Models/SendMetaTxnArgs.h"
#include "Models/SendMetaTxnResponse.h"
#include "UObject/Object.h"
#include "Util/Async.h"
#include "SequenceRelayer.generated.h"

class HttpHandler;

UCLASS()
class SEQUENCEPLUGIN_API USequenceRelayer : public UObject
{
	GENERATED_BODY()

public:
	void Relay(const FString& To, const FString& Data, const FString& Quote, const TArray<FIntentPrecondition>& Preconditions, TSuccessCallback<FString> OnSuccess, const TFunction<void(FString)>& OnFailure) const;
	void GetFeeOptions(const FFeeOptionsArgs& Args, TSuccessCallback<FFeeOptionsResponse> OnSuccess, const TFunction<void(FString)>& OnFailure) const;
	void GetMetaTxnReceipt(const FString& MetaTxID, TSuccessCallback<FGetMetaTxnReceiptResponse> OnSuccess, const TFunction<void(FString)>& OnFailure) const;
	void SendMetaTxn(const FSendMetaTxnArgs& Args, TSuccessCallback<FSendMetaTxnResponse> OnSuccess, const TFunction<void(FString)>& OnFailure) const;
	
private:
	void SendRequest(const FString& Endpoint, const FString& Payload, TSuccessCallback<FString> OnSuccess, TFunction<void(FString)> OnFailure) const;
	static FString BuildUrl();
};
