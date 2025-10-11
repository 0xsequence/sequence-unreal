#include "TransactionService.h"
#include "SignatureService.h"
#include "EcosystemWallet/Primitives/Envelope/Envelope.h"
#include "Signers/WalletSessions.h"
#include "Storage/SessionStorage.h"

void FTransactionService::SignAndBuild(FBigInt ChainId, const TArray<FCall>& Calls, const bool CheckDeployed, TFunction<void(TTuple<FString, TArray<uint8>>)> OnSuccess, TFunction<void()> OnFailure)
{
	FWalletState* CurrentWalletState = this->WalletState.Get();
	TSharedPtr<FCalls> Payload = MakeShared<FCalls>(Calls, FBigInt("0"), FBigInt(CurrentWalletState->Nonce));
	TSharedPtr<FEnvelope> Envelope = MakeShared<FEnvelope>(ChainId, CurrentWalletState->Address, CurrentWalletState->Config, Payload);

	TArray<FSessionCredentials> Credentials = USessionStorage::GetStoredSessions().Sessions;
	
	TArray<FSessionSigner> Signers;
	for (FSessionCredentials Credential : Credentials)
	{
		FSessionSigner Signer = FSessionSigner();
		Signer.Initialize(Credential);
		Signers.Add(Signer);
	}
	
	FSignatureService SignatureService = FSignatureService(ChainId, CurrentWalletState->SessionsImageHash, Envelope,
		CurrentWalletState->ConfigUpdates, Signers, CurrentWalletState->SessionsTopology);

	SignatureService.SignCalls([OnSuccess](const FRawSignature& Signature)
	{
		
	}, OnFailure);
}
