#include "RelayerReceiptPoller.h"
#include "Engine/Engine.h"

void URelayerReceiptPoller::StartPolling(const FString& Hash, const TFunction<void(FString)>& OnSuccess, const TFunction<void(FString)>& OnFailure)
{
	this->Relayer = NewObject<USequenceRelayer>();
	this->OnSuccess = OnSuccess;
	this->OnFailure = OnFailure;
	
	CurrentStatus = "Pending";
	PollRecursive(Hash);
}

void URelayerReceiptPoller::PollRecursive(const FString& Hash)
{
	if (Relayer == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Relayer is null, cannot poll receipt."))
		return;
	}
	
	Relayer->GetMetaTxnReceipt(Hash, [this, Hash](const FGetMetaTxnReceiptResponse& Response){
		CurrentStatus = Response.Receipt.Status;

		if (CurrentStatus == "SUCCEEDED" || CurrentStatus == "FAILED")
		{
			OnSuccess(Response.Receipt.TxnHash);
			return;
		}

		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, Hash]()
		{
			FPlatformProcess::Sleep(2.0f);
			AsyncTask(ENamedThreads::GameThread, [this, Hash]()
			{
				PollRecursive(Hash);
			});
		});
	}, OnFailure);
}