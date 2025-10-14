#include "TransactionService.h"
#include "SignatureService.h"
#include "EcosystemWallet/Primitives/Envelope/Envelope.h"

void FTransactionService::SignAndBuild(FBigInt ChainId, const TArray<FCall>& Calls, const bool CheckDeployed, TFunction<void(TTuple<FString, TArray<uint8>>)> OnSuccess, TFunction<void(FString)> OnFailure)
{
	FWalletState* CurrentWalletState = this->WalletState.Get();
	TSharedPtr<FCalls> Payload = MakeShared<FCalls>(Calls, FBigInt("0"), FBigInt(CurrentWalletState->Nonce));
	TSharedPtr<FEnvelope> Envelope = MakeShared<FEnvelope>(ChainId, CurrentWalletState->Address, CurrentWalletState->Config, Payload);

	TSharedPtr<FSignatureService> SignatureService = MakeShared<FSignatureService>(FSignatureService(ChainId, CurrentWalletState->SessionsImageHash, Envelope,
		CurrentWalletState->ConfigUpdates, this->Signers, CurrentWalletState->SessionsTopology));

	SignatureService->SignCalls([OnSuccess](const TSharedPtr<FRawSignature>& Signature)
	{
		const TArray<uint8> EncodedSignature = Signature->Encode();
		UE_LOG(LogTemp, Display, TEXT("Received signature: %s"), *FByteArrayUtils::BytesToHexString(EncodedSignature));
	}, OnFailure);
}
