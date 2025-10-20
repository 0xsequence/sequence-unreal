#pragma once
#include "WalletState.h"
#include "EcosystemWallet/Primitives/Calls/Call.h"
#include "EcosystemWallet/Signers/UsageLimit.h"
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
	void PrepareIncrement(
		const TSharedPtr<FTransactionService>& Service,
		const FBigInt& ChainId,
		const TArray<FCall>& Calls,
		const TFunction<void(TSharedPtr<FTransactionService>, TSharedPtr<FCall>)>& OnCompleted);

	static void WaitForAllIncrements(
		const FBigInt& ChainId,
		const TSharedPtr<FSessionsTopology>& SessionsTopology,
		const TMap<TSharedPtr<FSessionSigner>, TArray<FCall>>& SignersToCallsMap,
		TFunction<void(const TArray<TSharedPtr<FUsageLimit>>&)> OnAllComplete);
	
	TArray<FSessionSigner> Signers;
	TSharedPtr<FWalletState> WalletState;
};
