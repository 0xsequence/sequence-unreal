#pragma once
#include "EcosystemWallet/Primitives/Attestation/Attestation.h"
#include "EcosystemWallet/Primitives/Calls/Call.h"

class SEQUENCEPLUGIN_API FImplicitRequestEncoder
{
public:
	static FString EncodeAcceptImplicitRequest(const FCall& Call, const FString& ParentAddress, const FAttestation& Attestation);
};
