#pragma once

#include "CoreMinimal.h"
#include "Checkout/Transak/Structs/TransakNFTData.h"

/**
 * Utility class to encode Transak NFT data into a URL-friendly string.
 */
class SEQUENCEPLUGIN_API FTransakNFTDataEncoder
{
public:
    static FString Encode(const FTransakNFTData& NftData);
};