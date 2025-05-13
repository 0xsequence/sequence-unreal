// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "Types/BinaryData.h"

// Derives a public key from a private key 
FPublicKey SEQUENCEPLUGIN_API GetPublicKey(FPrivateKey PrivateKey);

// Derives an address from the public key
FAddress SEQUENCEPLUGIN_API GetAddress(FPublicKey PublicKey);

// Finds a keccak hash for some binary data
FHash256 SEQUENCEPLUGIN_API GetKeccakHash(FBinaryData &Data);

// Derives contract address from a given sending address and nonce
FAddress SEQUENCEPLUGIN_API GetContractAddress(FAddress Sender, FBlockNonce Nonce);