#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Util/Async.h"
#include "SequenceRelayer.generated.h"

class HttpHandler;

UCLASS()
class SEQUENCEPLUGIN_API USequenceRelayer : public UObject
{
	GENERATED_BODY()

public:
	void Relay(const FString& WalletAddress, TSuccessCallback<FString> OnSuccess, const TFunction<void(FString)>& OnFailure) const;
	void GetFeeOptions(const FString& WalletAddress, TSuccessCallback<FString> OnSuccess, const TFunction<void(FString)>& OnFailure) const;
	void GetMetaTxnReceipt(const FString& WalletAddress, TSuccessCallback<FString> OnSuccess, const TFunction<void(FString)>& OnFailure) const;
	void SendMetaTxn(const FString& WalletAddress, TSuccessCallback<FString> OnSuccess, const TFunction<void(FString)>& OnFailure) const;
	
private:
	void SendRequest(const FString& Endpoint, const FString& Payload, TSuccessCallback<FString> OnSuccess, TFunction<void(FString)> OnFailure) const;
	static FString BuildUrl();
};
