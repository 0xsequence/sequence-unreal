#include "Types/Wallet.h"
#include "Eth/EthTransaction.h"

FWallet::FWallet(const FPrivateKey PrivateKey) : PrivateKey(PrivateKey)
{
}

FUnsizedData FWallet::GetSignedTransaction(FEthTransaction Transaction, const int ChainId) const
{
	return Transaction.GetSignedTransaction(PrivateKey, ChainId);
}
