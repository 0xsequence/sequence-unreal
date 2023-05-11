#pragma once
#include "BinaryData.h"
#include "Types.h"

class Crypto
{
public:
	
};

FPublicKey GetPublicKey(FPrivateKey PrivateKey);
FAddress GetAddress(FPublicKey PublicKey);