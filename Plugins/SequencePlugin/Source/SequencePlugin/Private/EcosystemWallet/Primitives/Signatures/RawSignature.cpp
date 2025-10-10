#include "EcosystemWallet/Primitives/Signatures/RawSignature.h"

TArray<uint8> FRawSignature::Encode(bool SkipCheckpointerData, bool SkipCheckpointerAddress)
{
    return TArray<uint8>();
}

TSharedPtr<FRawSignature> FRawSignature::Decode(const TArray<uint8>& Erc6492Signature)
{
    return MakeShareable(new FRawSignature());
}
