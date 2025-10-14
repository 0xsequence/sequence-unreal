#include "TransactionService.h"
#include "SignatureService.h"
#include "EcosystemWallet/Primitives/Envelope/Envelope.h"
#include "Relayer/SequenceRelayer.h"

void FTransactionService::SignAndBuild(FBigInt ChainId, const TArray<FCall>& Calls, const bool CheckDeployed, TFunction<void(TTuple<FString, TArray<uint8>>)> OnSuccess, TFunction<void(FString)> OnFailure)
{
	FWalletState* CurrentWalletState = this->WalletState.Get();
	TSharedPtr<FCalls> Payload = MakeShared<FCalls>(Calls, FBigInt("0"), FBigInt(CurrentWalletState->Nonce));
	TSharedPtr<FEnvelope> Envelope = MakeShared<FEnvelope>(ChainId, CurrentWalletState->Address, CurrentWalletState->Config, Payload);

	TSharedPtr<FSignatureService> SignatureService = MakeShared<FSignatureService>(FSignatureService(ChainId, CurrentWalletState->SessionsImageHash, Envelope,
		CurrentWalletState->ConfigUpdates, this->Signers, CurrentWalletState->SessionsTopology));

	SignatureService->SignCalls([Payload, CurrentWalletState, OnSuccess, OnFailure](const TSharedPtr<FRawSignature>& Signature)
	{
		const FString EncodedPayload = FByteArrayUtils::BytesToHexString(Payload->Encode());
		const FString EncodedSignature = FByteArrayUtils::BytesToHexString(Signature->Encode());
		
		UE_LOG(LogTemp, Display, TEXT("Final encoded calls: %s"), *EncodedPayload);
		UE_LOG(LogTemp, Display, TEXT("Final encoded signature: %s"), *EncodedSignature);

		const FString ExecuteValues = FString::Printf(TEXT("[\"%s\",\"%s\"]"), *EncodedPayload, *EncodedSignature);
		const FString EncodedExecute = FEthAbiBridge::EncodeFunctionCall("execute(bytes,bytes)", ExecuteValues);

		const USequenceRelayer* Relayer = NewObject<USequenceRelayer>();
		Relayer->Relay(CurrentWalletState->Address, EncodedExecute, "", TArray<FIntentPrecondition>(), [Relayer, OnSuccess, OnFailure](const FString& Receipt)
		{
			Relayer->GetMetaTxnReceipt(Receipt, [](const FGetMetaTxnReceiptResponse& Response)
			{
				const FString TxnHash = Response.Receipt.TxnHash;
				UE_LOG(LogTemp, Display, TEXT("TxnHash: %s"), *TxnHash);
			}, OnFailure);
		}, OnFailure);
	}, OnFailure);
}
