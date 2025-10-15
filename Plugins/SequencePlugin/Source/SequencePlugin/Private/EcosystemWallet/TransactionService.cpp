#include "TransactionService.h"
#include "SignatureService.h"
#include "EcosystemWallet/Primitives/Envelope/Envelope.h"

void FTransactionService::SignAndBuild(FBigInt ChainId, const TArray<FCall>& Calls, const bool CheckDeployed, TFunction<void(TTuple<FString, FString>)> OnSuccess, TFunction<void(FString)> OnFailure)
{
	FWalletState* CurrentWalletState = this->WalletState.Get();
	TSharedPtr<FCalls> Payload = MakeShared<FCalls>(Calls, FBigInt("0"), FBigInt(CurrentWalletState->Nonce));
	TSharedPtr<FEnvelope> Envelope = MakeShared<FEnvelope>(ChainId, CurrentWalletState->Address, CurrentWalletState->Config, Payload);

	TSharedPtr<FSignatureService> SignatureService = MakeShared<FSignatureService>(FSignatureService(ChainId, CurrentWalletState->SessionsImageHash, Envelope,
		CurrentWalletState->ConfigUpdates, this->Signers, CurrentWalletState->SessionsTopology));

	SignatureService->SignCalls([Payload, CurrentWalletState, CheckDeployed, OnSuccess](const TSharedPtr<FRawSignature>& Signature)
	{
		const FString EncodedPayload = FByteArrayUtils::BytesToHexString(Payload->Encode());
		const FString EncodedSignature = FByteArrayUtils::BytesToHexString(Signature->Encode());
		
		const FString ExecuteValues = FString::Printf(TEXT("[\"%s\",\"%s\"]"), *EncodedPayload, *EncodedSignature);
		const FString EncodedExecute = FEthAbiBridge::EncodeFunctionCall("execute(bytes,bytes)", ExecuteValues);

		if (CurrentWalletState->IsDeployed || !CheckDeployed)
		{
			OnSuccess(TTuple<FString, FString>(CurrentWalletState->Address, EncodedExecute));
			return;
		}

		const TArray<uint8> DeployData = FErc6492::CreateDeployData(CurrentWalletState->DeployContext.MainModule, CurrentWalletState->DeployHash);
		const FString DeployDataHex = FByteArrayUtils::BytesToHexString(DeployData);

		const TArray<FCall> DeployCalls {
			FCall {CurrentWalletState->DeployContext.Factory, DeployDataHex, FBigInt("0"), FBigInt("0"), false, false, "1"},
			FCall {CurrentWalletState->Address, EncodedExecute, FBigInt("0"), FBigInt("0"), false, false, "1"}
		};

		FCalls DeployPayload = FCalls(DeployCalls, FBigInt("0"), FBigInt("0"));
		const FString EncodedDeployPayload = FByteArrayUtils::BytesToHexString(DeployPayload.Encode());

		OnSuccess(TTuple<FString, FString>(CurrentWalletState->DeployContext.GuestModule, EncodedDeployPayload));
	}, OnFailure);
}
