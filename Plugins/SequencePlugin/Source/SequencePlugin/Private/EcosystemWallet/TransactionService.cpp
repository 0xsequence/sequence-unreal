#include "TransactionService.h"
#include "EcosystemWallet/Primitives/Envelope/Envelope.h"

TTuple<FString, TArray<uint8>> FTransactionService::SignAndBuild(FBigInt ChainId, const TArray<FCall>& Calls,
                                                                 const bool CheckDeployed)
{
	FWalletState* CurrentWalletState = this->WalletState.Get();
	TSharedPtr<FCalls> Payload = MakeShared<FCalls>(Calls, FBigInt("0"), FBigInt(CurrentWalletState->Nonce));
	TSharedPtr<FEnvelope> Envelope = MakeShared<FEnvelope>(ChainId, CurrentWalletState->Address, CurrentWalletState->Config, Payload);

	TTuple<FString, TArray<uint8>> Return = TTuple<FString, TArray<uint8>>();
	return Return;
}
