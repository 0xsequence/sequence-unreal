#include "SessionCallSignature.h"

TArray<uint8> FSessionCallSignature::EncodeSignatures(
		const TArray<FSessionCallSignature>& Signatures,
		const TSharedPtr<FSessionsTopology>& SessionsTopology,
		const TArray<FString>& ImplicitSigners,
		const TArray<FString>& ExplicitSigners)
{
	return TArray<uint8>();
}
