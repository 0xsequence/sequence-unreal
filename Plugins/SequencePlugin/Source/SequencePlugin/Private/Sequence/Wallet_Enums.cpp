#include "Sequence/Wallet_Enums.h"

template<>
FString UMarketplaceWalletKindExtensions::AsString<EWalletKind>(const EWalletKind OrderbookKind)
{
	switch (OrderbookKind)
	{
		case EWalletKind::Sequence:
			return FString("sequence");
		default:
			return FString("unknown");
	}
}