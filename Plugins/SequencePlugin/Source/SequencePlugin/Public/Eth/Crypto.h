// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "Types/BinaryData.h"
#include "Types/Types.h"

// Derives a public key from a private key 
FPublicKey GetPublicKey(FPrivateKey PrivateKey);

// Derives an address from the public key
FAddress GetAddress(FPublicKey PublicKey);

// Finds a keccak hash for some binary data
FHash256 GetKeccakHash(FBinaryData &Data);

// Derives contract address from a given sending address and nonce
FAddress GetContractAddress(FAddress Sender, FBlockNonce Nonce);