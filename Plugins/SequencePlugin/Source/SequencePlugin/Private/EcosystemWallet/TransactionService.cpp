#include "TransactionService.h"

#include "RelayerReceiptPoller.h"
#include "SignatureService.h"
#include "EcosystemWallet/Primitives/Envelope/Envelope.h"
#include "Relayer/SequenceRelayer.h"

void FTransactionService::SignAndBuild(FBigInt ChainId, const TArray<FCall>& Calls, const bool CheckDeployed, TFunction<void(TTuple<FString, FString>)> OnSuccess, TFunction<void(FString)> OnFailure)
{
	FWalletState* CurrentWalletState = this->WalletState.Get();
	TSharedPtr<FCalls> Payload = MakeShared<FCalls>(Calls, FBigInt("0"), FBigInt(CurrentWalletState->Nonce));
	TSharedPtr<FEnvelope> Envelope = MakeShared<FEnvelope>(ChainId, CurrentWalletState->Address, CurrentWalletState->Config, Payload);

	TSharedPtr<FSignatureService> SignatureService = MakeShared<FSignatureService>(FSignatureService(ChainId, CurrentWalletState->SessionsImageHash, Envelope,
		CurrentWalletState->ConfigUpdates, this->Signers, CurrentWalletState->SessionsTopology));

	SignatureService->SignCalls([Payload, CurrentWalletState, OnSuccess](const TSharedPtr<FRawSignature>& Signature)
	{
		const FString EncodedPayload = FByteArrayUtils::BytesToHexString(Payload->Encode());
		const FString EncodedSignature = FByteArrayUtils::BytesToHexString(Signature->Encode());
		
		const FString ExecuteValues = FString::Printf(TEXT("[\"%s\",\"%s\"]"), *EncodedPayload, *EncodedSignature);
		const FString EncodedExecute = FEthAbiBridge::EncodeFunctionCall("execute(bytes,bytes)", ExecuteValues);

		OnSuccess(TTuple<FString, FString>(CurrentWalletState->Address, EncodedExecute));
	}, OnFailure);
}
