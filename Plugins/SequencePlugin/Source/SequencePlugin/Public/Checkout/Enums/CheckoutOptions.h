#pragma once
#include "Marketplace/TransactionCrypto.h"
#include "Marketplace/TransactionNFTCheckoutProvider.h"
#include "Marketplace/TransactionOnRampProvider.h"
#include "Marketplace/TransactionSwapProvider.h"

#include "CheckoutOptions.generated.h"

ENUM_RANGE_BY_COUNT(ECheckoutOptions, 5)
UENUM(Blueprintable)
enum class ECheckoutOptions : uint8
{
	CryptoPurchase			UMETA(DisplayName = "CryptoPurchase"),
	SwapAndPay				UMETA(DisplayName = "SwapAndPay"),
	SardineCheckout			UMETA(DisplayName = "SardineCheckout"),
	TransferFundsViaQR		UMETA(DisplayName = "TransferFundsViaQR"),
	TransakOnramp			UMETA(DisplayName = "TransakOnramp")
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FCheckoutOptions
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ETransactionCrypto Crypto;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<ETransactionSwapProvider> Swap;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<ETransactionNftCheckoutProvider> NftCheckout;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<ETransactionOnRampProvider> OnRamp;

	FCheckoutOptions()
	{
		this->Crypto = ETransactionCrypto::Unknown;
		this->Swap = TArray{ ETransactionSwapProvider::Unknown };
		this->NftCheckout = TArray{ ETransactionNftCheckoutProvider::Unknown };
		this->OnRamp = TArray{ ETransactionOnRampProvider::Unknown };
	}

	FCheckoutOptions(const ETransactionCrypto Crypto, const TArray<ETransactionSwapProvider>& Swap, const TArray<ETransactionNftCheckoutProvider>& NftCheckout, const TArray<ETransactionOnRampProvider>& OnRamp)
	{
		this->Crypto = Crypto;
		this->Swap = Swap;
		this->NftCheckout = NftCheckout;
		this->OnRamp = OnRamp;
	}

	bool customConstructor = false;

	void Construct(FJsonObject JSON_In){} 
	
	void Setup(const FJsonObject& JSON_In)
	{
		if (JSON_In.HasField(TEXT("crypto")))
		{
			this->Crypto = UTransactionCrypto::GetTransactionCryptoFromString(JSON_In.GetStringField(TEXT("crypto")));
		}
		if (JSON_In.HasField(TEXT("swap")))
		{
			this->Swap.Reset();
			const TArray<TSharedPtr<FJsonValue>>& SwapJsonValues = JSON_In.GetArrayField(TEXT("swap"));
			FString SwapElementValue = TEXT("");
			for (int32 i = 0; i < SwapJsonValues.Num(); i++)
			{
				if (SwapJsonValues[i]->TryGetString(SwapElementValue))
				{
					ETransactionSwapProvider NewSwapValue = UTransactionSwapProvider::GetTransactionSwapFromString(SwapElementValue);
					this->Swap.Add(NewSwapValue);
				}
			}
		}
		if (JSON_In.HasField(TEXT("nftCheckout")))
		{
			this->NftCheckout.Reset();
			const TArray<TSharedPtr<FJsonValue>>& NftCheckoutJsonValues = JSON_In.GetArrayField(TEXT("nftCheckout"));
			FString NftCheckoutElementValue = TEXT("");
			for (int32 i = 0; i < NftCheckoutJsonValues.Num(); i++)
			{
				if (NftCheckoutJsonValues[i]->TryGetString(NftCheckoutElementValue))
				{
					ETransactionNftCheckoutProvider NewNftCheckoutValue = UTransactionNftCheckoutProvider::GetTransactionNftCheckoutFromString(NftCheckoutElementValue);
					this->NftCheckout.Add(NewNftCheckoutValue);
				}
			}
		}
		if (JSON_In.HasField(TEXT("onRamp")))
		{
			this->OnRamp.Reset();
			const TArray<TSharedPtr<FJsonValue>>& OnRampJsonValues = JSON_In.GetArrayField(TEXT("onRamp"));
			FString OnRampElementValue = TEXT("");
			for (int32 i = 0; i < OnRampJsonValues.Num(); i++)
			{
				if (OnRampJsonValues[i]->TryGetString(OnRampElementValue))
				{
					ETransactionOnRampProvider NewOnRampValue = UTransactionOnRampProvider::GetTransactionOnRampFromString(OnRampElementValue);
					this->OnRamp.Add(NewOnRampValue);
				}
			}
		}
	}
};

UCLASS()
class SEQUENCEPLUGIN_API UCheckoutOptionsExtensions : public UObject
{
	GENERATED_BODY()

public:
	static ECheckoutOptions GetCheckoutOptionsFromString(const FString& CheckoutOptionsString)
	{
		FText EnumDisplayText = FText::FromString("");
		for (const ECheckoutOptions Val : TEnumRange<ECheckoutOptions>())
		{
			UEnum::GetDisplayValueAsText(Val, EnumDisplayText);
			
			if (UEnum::GetValueAsString(Val).Equals(CheckoutOptionsString) || EnumDisplayText.ToString().Equals(CheckoutOptionsString))
			{
				return Val;
			}
		}

		// Fallback if nothing is found
		constexpr ECheckoutOptions CheckoutOptions{ ECheckoutOptions::CryptoPurchase };
		return CheckoutOptions;
	}
	
	static FString AsString(const ECheckoutOptions CheckoutOptionsValue)
	{
		switch (CheckoutOptionsValue)
		{
			case ECheckoutOptions::CryptoPurchase: return "cryptoPurchase";
			case ECheckoutOptions::SwapAndPay: return "swapAndPay";
			case ECheckoutOptions::SardineCheckout: return "sardineCheckout";
			case ECheckoutOptions::TransferFundsViaQR: return "transferFundsViaQR";
			case ECheckoutOptions::TransakOnramp: return "transakOnramp";
			default: return "cryptoPurchase";
		}
	}

};