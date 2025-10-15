#pragma once
#include "WalletState.h"
#include "EcosystemWallet/Primitives/Calls/Call.h"
#include "Signers/SessionSigner.h"
#include "Types/BigInt.h"

class SEQUENCEPLUGIN_API FTransactionService
{
public:
	explicit FTransactionService(const TArray<FSessionSigner>& InSigners, const TSharedPtr<FWalletState>& InWalletState)
		: Signers(InSigners), WalletState(InWalletState) {};
	
	void SignAndBuild(FBigInt ChainId, const TArray<FCall>& Calls, const bool CheckDeployed,
		TFunction<void(TTuple<FString, FString>)> OnSuccess, TFunction<void(FString)> OnFailure);

private:
	TArray<FSessionSigner> Signers;
	TSharedPtr<FWalletState> WalletState;
};
