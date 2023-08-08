#pragma once
#include "Core.h"

class V2
{
public:
	
};

FSignature DecodeSignature(FUnsizedData Data);
FWalletConfig DecodeWalletConfig(FUnsizedData Data);

enum class ESignatureType
{
	Legacy = 0,
	Regular = 1,
	NoChainID = 2,
	Chained = 3,
};

class SignatureTree
{
	
};

class FRegularSignature
{
	bool IsRegular;
	uint16 Threshold;
	uint32 Checkpoint;
	
public:
	FRegularSignature(bool bIsRegular, uint16 Threshold, uint32 Checkpoint)
		: IsRegular(bIsRegular),
		  Threshold(Threshold),
		  Checkpoint(Checkpoint)
	{
	}
};

