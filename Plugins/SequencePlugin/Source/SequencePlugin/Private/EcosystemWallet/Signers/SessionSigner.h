#pragma once

#include "CoreMinimal.h"
#include "SessionCredentials.h"
#include "EcosystemWallet/Primitives/Calls/Call.h"
#include "EcosystemWallet/Primitives/Sessions/SessionsTopology.h"
#include "EcosystemWallet/Primitives/Signatures/SessionCallSignature.h"
#include "Types/BigInt.h"
#include "UObject/Object.h"

class SEQUENCEPLUGIN_API FSessionSigner
{
public:
	void Initialize(const FSessionCredentials& NewCredentials);

	FString GetIdentitySigner();
	
	void IsSupportedCall(const FCall& Call, TFunction<void(bool)> OnCompleted);
	
	FSessionCallSignature SignCall(const FBigInt& ChainId, const FCall& Call, const int32& CallIdx,
		const TSharedPtr<FSessionsTopology> SessionsTopology, const FBigInt& Space, const FBigInt& Nonce);

	bool IsExplicit;
	FString Address;

private:
	FSessionCredentials Credentials;
};
