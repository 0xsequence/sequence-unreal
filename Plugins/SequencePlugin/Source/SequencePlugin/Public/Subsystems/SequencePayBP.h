// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Sequence/SequencePay.h"
#include "Sequence/SequenceWalletBPTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Marketplace/Structs/SeqGetSwapPriceResponse.h"
#include "UObject/Object.h"
#include "SequencePayBP.generated.h"

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequencePayBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSupportedTransakCountries, const TArray<FSupportedCountry>&, SupportedCountries);
	DECLARE_DYNAMIC_DELEGATE_FiveParams(FOnGetSwapPrice, FString, CurrencyAddress, FString, CurrencyBalance, FString, Price, FString, MaxPrice, FString, TransactionValue);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetSwapPrices, const TArray<FSeqSwapPrice>&, SwapPrices);
	DECLARE_DYNAMIC_DELEGATE_EightParams(FOnGetSwapQuote, FString, CurrencyAddress, FString, CurrencyBalance, FString, Price, FString, MaxPrice, FString, To, FString, TransactionData, FString, TransactionValue, FString, ApproveData);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFailure, const FString&, Error);

public:
	USequencePayBP();
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Pay")
	FString GetOnRampLink(const FString& WalletAddress, const FTransakSettings& Settings);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Pay")
	void OpenOnRampInExternalBrowser(const FString& WalletAddress, const FTransakSettings& Settings);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Pay")
	void GetSupportedOnRampCountries(FOnSupportedTransakCountries OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Pay")
	void GetSwapPrice(const FString& SellCurrency, const FString& BuyCurrency, const FString& BuyAmount, const int SlippagePercentage, FOnGetSwapPrice OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Pay")
	void GetSwapPrices(const FString& UserWallet, const FString& BuyCurrency, const FString& BuyAmount, const int SlippagePercentage, FOnGetSwapPrices OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Pay")
	void GetSwapQuote(const FString& UserWallet, const FString& BuyCurrency, const FString& SellCurrency, const FString& BuyAmount, const bool IncludeApprove, const int SlippagePercentage, FOnGetSwapQuote OnSuccess, FOnFailure OnFailure);

private:
	UPROPERTY()
	USequencePay* Pay;
};
