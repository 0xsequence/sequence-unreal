#pragma once

#include "CoreMinimal.h"
#include "Checkout/Transak/Structs/TransakNFTData.h"

namespace TransakNFTDataEncoder
{
	FString Encode(const FTransakNFTData& NftData);
}
