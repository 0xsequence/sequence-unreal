#pragma once
#include "BinaryData.h"
#include "EthTransaction.h"

class FWallet
{
	FPrivateKey PrivateKey;
public:
	FWallet(FPrivateKey PrivateKey);
	FNonUniformData GetSignedTransaction(FEthTransaction Transaction, int ChainId) const;
};
