#pragma once
#include "BinaryData.h"
#include "FEthTransaction.h"

class FWallet
{
	FPrivateKey PrivateKey;
public:
	FWallet(FPrivateKey PrivateKey);
	FNonUniformData GetSignedTransaction(FEthTransaction Transaction, int ChainId) const;
};
