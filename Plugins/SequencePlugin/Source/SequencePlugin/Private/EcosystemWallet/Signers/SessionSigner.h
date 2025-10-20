#pragma once

#include "CoreMinimal.h"
#include "SessionCredentials.h"
#include "EcosystemWallet/Primitives/Calls/Call.h"
#include "EcosystemWallet/Primitives/Sessions/SessionsTopology.h"
#include "EcosystemWallet/Primitives/Signatures/SessionCallSignature.h"
#include "EcosystemWallet/Signers/UsageLimit.h"
#include "Types/BigInt.h"
#include "UObject/Object.h"

class SEQUENCEPLUGIN_API FSessionSigner
{
public:
	explicit FSessionSigner(const FSessionCredentials& NewCredentials) : Credentials(NewCredentials) { }

	FString GetIdentitySigner();
	FString GetSessionAddress() const;
	
	bool IsSupportedCall(const FBigInt& ChainId, const FCall& Call, const TArray<FSessionSigner>& Signers, const TSharedPtr<FSessionsTopology>& SessionsTopology);
	
	TSharedPtr<FSessionCallSignature> SignCall(const FBigInt& ChainId, const FCall& Call, const int32& CallIdx,
		const TSharedPtr<FSessionsTopology>& SessionsTopology, const FBigInt& Space, const FBigInt& Nonce);

	void PrepareIncrements(
		const FBigInt& ChainId,
		const TArray<FCall>& Calls,
		const TSharedPtr<FSessionsTopology>& SessionsTopology,
		const TFunction<void(TSharedPtr<FUsageLimit>)>& OnCompleted);

	FSessionCredentials Credentials;

	bool operator==(const FSessionSigner& Other) const
	{
		return GetSessionAddress() == Other.GetSessionAddress();
	}

private:
	int32 FindSupportedPermission(const FBigInt& ChainId, const FCall& Call, const TSharedPtr<FSessionsTopology>& SessionsTopology);
	TArray<uint8> HashCallWithReplayProtection(const FBigInt& ChainId, const FCall&, const uint32& CallIdx, const FBigInt& Space, const FBigInt& Nonce);
	FString GenerateImplicitRequestMagic();
	
	void GetCurrentUsageLimit(const FString& ValueUsageHash, TFunction<void(FBigInt)> OnCompleted);

	bool CheckCallForIncrementUsageLimit(const FCall& Call);
	FString GetValueUsageHash();
	
	void CheckAcceptImplicitRequest(const FCall& Call, TFunction<void(bool)> OnCompleted);
};

FORCEINLINE uint32 GetTypeHash(const FSessionSigner& Signer)
{
	return GetTypeHash(Signer.GetSessionAddress());
}
