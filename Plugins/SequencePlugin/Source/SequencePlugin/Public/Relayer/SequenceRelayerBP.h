#pragma once

#include "CoreMinimal.h"
#include "SequenceRelayer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SequenceRelayerBP.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API USequenceRelayerBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	DECLARE_DYNAMIC_DELEGATE(FOnSuccess);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRelaySuccess, FString, TxnHash);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetFeeOptionsSuccess, FFeeOptionsResponse, Data);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetMetaTxnReceiptSuccess, FGetMetaTxnReceiptResponse, Data);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSendMetaTxnSuccess, FSendMetaTxnResponse, Data);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFailure, FString, Error);
	
	USequenceRelayerBP();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK")
	void Relay(const FString& To, const FString& Data, const FString& Quote, const TArray<FIntentPrecondition>& Preconditions, FOnRelaySuccess OnSuccess, FOnFailure OnFailure);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK")
	void GetFeeOptions(const FFeeOptionsArgs& Args, FOnGetFeeOptionsSuccess OnSuccess, FOnFailure OnFailure);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK")
	void GetMetaTxnReceipt(const FString& MetaTxID, FOnGetMetaTxnReceiptSuccess OnSuccess, FOnFailure OnFailure);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK")
	void SendMetaTxn(const FSendMetaTxnArgs& Args, FOnSendMetaTxnSuccess OnSuccess, FOnFailure OnFailure);

private:
	UPROPERTY()
	USequenceRelayer* Relayer;
};
