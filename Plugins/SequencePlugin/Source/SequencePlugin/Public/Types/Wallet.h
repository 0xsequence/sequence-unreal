#pragma once
#include "BinaryData.h"
#include "EthTransaction.h"

class FWallet
{
	FPrivateKey PrivateKey;
public:
	FWallet(FPrivateKey PrivateKey);
	FUnsizedData GetSignedTransaction(FEthTransaction Transaction, int ChainId) const;
};
