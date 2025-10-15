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
	explicit FSessionSigner(const FSessionCredentials& NewCredentials) : Credentials(NewCredentials) { }

	FString GetIdentitySigner();
	
	void IsSupportedCall(const FBigInt& ChainId, const FCall& Call, const TSharedPtr<FSessionsTopology>& SessionsTopology, TFunction<void(bool)> OnCompleted);
	
	TSharedPtr<FSessionCallSignature> SignCall(const FBigInt& ChainId, const FCall& Call, const int32& CallIdx,
		const TSharedPtr<FSessionsTopology>& SessionsTopology, const FBigInt& Space, const FBigInt& Nonce);

	FSessionCredentials Credentials;

private:
	int32 FindSupportedPermission(const FBigInt& ChainId, const FCall& Call, const TSharedPtr<FSessionsTopology>& SessionsTopology);
	TArray<uint8> HashCallWithReplayProtection(const FBigInt& ChainId, const FCall&, const uint32& CallIdx, const FBigInt& Space, const FBigInt& Nonce);
};
