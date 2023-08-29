#pragma once
#include "BinaryData.h"
#include "Eth/EthTransaction.h"

class FWallet
{
	FPrivateKey PrivateKey;
public:
	FWallet(FPrivateKey PrivateKey);
	FUnsizedData GetSignedTransaction(FEthTransaction Transaction, int ChainId) const;
};
