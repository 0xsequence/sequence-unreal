#pragma once

#include "SequenceWallet.h"
#include "Wallet_Enums.generated.h"

UENUM(BlueprintType)
enum class EWalletKind : uint8
{
	Unknown  UMETA(DisplayName = "unknown"),
	Sequence UMETA(DisplayName = "sequence")
};

// PropertyType Extensions
UCLASS()
class UMarketplaceWalletKindExtensions : public UObject
{
	GENERATED_BODY()

public:
	template<typename WalletType>
		static EWalletKind GetWalletKind(WalletType)
	{
		if (std::is_same_v<WalletType, USequenceWallet>)
		{
			return EWalletKind::Sequence;
		}
		
		return EWalletKind::Unknown;
	}

	template<typename EnumType>
	static FString AsString(EnumType EnumValue);
};
