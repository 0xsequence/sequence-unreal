#pragma once
#include "EcosystemWallet/Primitives/Envelope/Envelope.h"
#include "EcosystemWallet/Primitives/Sessions/SessionsTopology.h"
#include "EcosystemWallet/Primitives/Signatures/RawSignature.h"
#include "EcosystemWallet/Primitives/Signatures/SignatureOfSapientSignerLeaf.h"
#include "EcosystemWallet/Signers/SessionSigner.h"
#include "KeyMachine/Models/ConfigUpdatesResponse.h"
#include "Types/BigInt.h"

class SEQUENCEPLUGIN_API FSignatureService
{
public:
	explicit FSignatureService(const FBigInt& InChainId, const FString& InImageHash, const TSharedPtr<FEnvelope>& InEnvelope,
		const TArray<FConfigUpdate>& InConfigUpdates, const TArray<FSessionSigner>& InSigners, const TSharedPtr<FSessionsTopology>& InSessionsTopology)
		: ChainId(InChainId), ImageHash(InImageHash), Envelope(InEnvelope), ConfigUpdates(InConfigUpdates), Signers(InSigners), SessionsTopology(InSessionsTopology) { }
	
	void SignCalls(const TFunction<void(FRawSignature)>& OnSuccess, const TFunction<void(FString)>& OnFailure);

private:
	void SignSapient(const TFunction<void(TSharedPtr<FSignatureOfSapientSignerLeaf>)>& OnSuccess, const TFunction<void(FString)>& OnFailure);

	FBigInt ChainId;
	FString ImageHash;
	TSharedPtr<FEnvelope> Envelope;
	TArray<FConfigUpdate> ConfigUpdates;
	TArray<FSessionSigner> Signers;
	TSharedPtr<FSessionsTopology> SessionsTopology;
};
