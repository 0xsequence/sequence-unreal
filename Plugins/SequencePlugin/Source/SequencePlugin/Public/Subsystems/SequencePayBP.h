#pragma once

#include "CoreMinimal.h"
#include "Sequence/SequencePay.h"
#include "Sequence/SequenceWalletBPTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Marketplace/Structs/SeqGetSwapPriceResponse.h"
#include "SequencePayBP.generated.h"

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequencePayBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSupportedTransakCountries, const TArray<FSupportedCountry>&, SupportedCountries);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetSupportedSwapChains, const TArray<int64>&, Chains);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetSupportedSwapTokens, const TArray<FSeqLifiToken>&, Tokens);
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnGetSwapPrice, FString, CurrencyAddress, int64, Price);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetSwapPrices, const TArray<FSeqSwapPrice>&, SwapPrices);
	DECLARE_DYNAMIC_DELEGATE_EightParams(FOnGetSwapQuote, FString, CurrencyAddress, FString, CurrencyBalance, FString, Price, FString, MaxPrice, FString, To, FString, TransactionData, FString, TransactionValue, FString, ApproveData);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFailure, const FString&, Error);
	
	USequencePayBP();
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Pay")
	FString GetOnRampLink(const FString& WalletAddress, const FTransakSettings& Settings);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Pay")
	void OpenOnRampInExternalBrowser(const FString& WalletAddress, const FTransakSettings& Settings);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Pay")
	void GetSupportedOnRampCountries(FOnSupportedTransakCountries OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Pay")
	void GetSupportedSwapChains(FOnGetSupportedSwapChains OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Pay")
	void GetSupportedSwapTokens(const TArray<int64>& ChainIds, FOnGetSupportedSwapTokens OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Pay")
	void GetSwapPrice(const FString& WalletAddress, const FString& SellCurrency, const FString& BuyCurrency, const FString& BuyAmount, FOnGetSwapPrice OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Pay")
	void GetSwapPrices(const FString& WalletAddress, const FString& BuyCurrency, const FString& BuyAmount, FOnGetSwapPrices OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Pay")
	void GetSwapQuote(const FString& WalletAddress, const FString& BuyCurrency, const FString& SellCurrency, const FString& BuyAmount, const FString& SellAmount, const bool IncludeApprove, FOnGetSwapQuote OnSuccess, FOnFailure OnFailure);

	USequencePay* GetSequencePay() const;
	
private:
	UPROPERTY()
	USequencePay* Pay;
};
