#pragma once
#include "WalletState.h"
#include "EcosystemWallet/Primitives/Calls/Call.h"
#include "Signers/SessionSigner.h"
#include "Types/BigInt.h"

class SEQUENCEPLUGIN_API FTransactionService
{
public:
	explicit FTransactionService(const TArray<TSharedPtr<FSessionSigner>>& InSigners, const TSharedPtr<FWalletState>& InWalletState)
		: Signers(InSigners), WalletState(InWalletState) {};
	
	void SignAndBuild(FBigInt ChainId, const TArray<FCall>& Calls, const bool CheckDeployed,
		TFunction<void(TTuple<FString, TArray<uint8>>)> OnSuccess, TFunction<void()> OnFailure);

private:
	TArray<TSharedPtr<FSessionSigner>> Signers;
	TSharedPtr<FWalletState> WalletState;
};
