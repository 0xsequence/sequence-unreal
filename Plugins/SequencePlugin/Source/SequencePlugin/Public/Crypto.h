#pragma once
#include "Types/BinaryData.h"
#include "Types/Types.h"

FPublicKey GetPublicKey(FPrivateKey PrivateKey);
FAddress GetAddress(FPublicKey PublicKey);
FHash256 GetKeccakHash(FBinaryData &Data);
FAddress GetContractAddress(FAddress Sender, FBlockNonce Nonce);