#pragma once
#include "EcosystemWallet/Primitives/Calls/Call.h"
#include "EcosystemWallet/Primitives/Sessions/SessionsTopology.h"
#include "EcosystemWallet/Signers/SessionSigner.h"
#include "Types/BigInt.h"

class SEQUENCEPLUGIN_API FSignerService
{
public:
	explicit FSignerService(const FBigInt& InChainId, const TArray<FCall>& InCalls,
		const TArray<FSessionSigner>& InSigners, const TSharedPtr<FSessionsTopology>& InSessionsTopology)
		: ChainId(InChainId), Calls(InCalls), Signers(InSigners), SessionsTopology(InSessionsTopology) { }
	
	void FindSignersForCalls(TFunction<void(TArray<FSessionSigner>)> OnSuccess, TFunction<void(FString)> OnFailure);

private:
	TArray<FSessionSigner> GetValidImplicitSigners(const FString& IdentitySigner, const TArray<FString>& Blacklist);
	TArray<FSessionSigner> GetValidExplicitSigners();
	TArray<FSessionSigner> FindSignerForEachCallAsync(const TArray<FSessionSigner>& ExplicitSigners, const TArray<FSessionSigner>& ImplicitSigners);
	
	FBigInt ChainId;
	TArray<FCall> Calls;
	TArray<FSessionSigner> Signers;
	TSharedPtr<FSessionsTopology> SessionsTopology;
};
