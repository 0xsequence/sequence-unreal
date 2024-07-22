// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Sequence/SequenceWalletBP.h"
#include "Sequence/SequenceAPI.h"

USequenceWalletBP::USequenceWalletBP()
{
}

void USequenceWalletBP::CallOnApiSignMessage(const FSequenceResponseStatus& Status, const FSignedMessage& SignedMessage) const
{
	if (this->OnApiSignMessage.IsBound())
		this->OnApiSignMessage.Broadcast(Status,SignedMessage);
	else
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: OnApiSignMessage]"));
}

void USequenceWalletBP::CallOnApiGetFilteredFeeOptions(const FSequenceResponseStatus& Status, const TArray<FFeeOption>& FeeOptions) const
{
	if (this->OnApiGetFilteredFeeOptions.IsBound())
		this->OnApiGetFilteredFeeOptions.Broadcast(Status,FeeOptions);
	else
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: OnApiGetFilteredFeeOptions]"));
}

void USequenceWalletBP::CallOnApiGetUnFilteredFeeOptions(const FSequenceResponseStatus& Status, const TArray<FFeeOption>& FeeOptions) const
{
	if (this->OnApiGetUnFilteredFeeOptions.IsBound())
		this->OnApiGetUnFilteredFeeOptions.Broadcast(Status,FeeOptions);
	else
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: OnApiGetUnFilteredFeeOptions]"));
}

void USequenceWalletBP::CallOnApiSendTransactionWithFee(const FSequenceResponseStatus& Status, const FTransactionResponse& Response) const
{
	if (this->OnApiSendTransactionWithFeeOption.IsBound())
		this->OnApiSendTransactionWithFeeOption.Broadcast(Status,Response);
	else
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: OnApiSendTransactionWithFeeOption]"));
}

void USequenceWalletBP::CallOnApiSendTransaction(const FSequenceResponseStatus& Status, const FTransactionResponse& Response) const
{
	if (this->OnApiSendTransaction.IsBound())
		this->OnApiSendTransaction.Broadcast(Status,Response);
	else
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: OnApiSendTransaction]"));
}

void USequenceWalletBP::CallOnApiListSessions(const FSequenceResponseStatus& Status, const TArray<FSession>& Sessions) const
{
	if (this->OnApiListSessions.IsBound())
		this->OnApiListSessions.Broadcast(Status,Sessions);
	else
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: OnApiListSessions]"));
}

void USequenceWalletBP::CallOnApiGetSupportedTransakCountries(const FSequenceResponseStatus& Status, const TArray<FSupportedCountry>& SupportedCountries) const
{
	if (this->OnApiGetSupportedTransakCountries.IsBound())
		this->OnApiGetSupportedTransakCountries.Broadcast(Status,SupportedCountries);
	else
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: OnApiGetSupportedTransakCountries]"));
}

void USequenceWalletBP::CallOnIndexerPing(const FSequenceResponseStatus& Status, bool PingResponse) const
{
	if (this->OnIndexerPing.IsBound())
		this->OnIndexerPing.Broadcast(Status,PingResponse);
	else
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: OnIndexerPing]"));
}

void USequenceWalletBP::CallOnIndexerVersion(const FSequenceResponseStatus& Status, const FVersion& Version) const
{
	if (this->OnIndexerVersion.IsBound())
		this->OnIndexerVersion.Broadcast(Status,Version);
	else
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: OnIndexerVersion]"));
}

void USequenceWalletBP::CallOnIndexerRuntimeStatus(const FSequenceResponseStatus& Status, const FRuntimeStatus& RuntimeStatus) const
{
	if (this->OnIndexerRuntimeStatus.IsBound())
		this->OnIndexerRuntimeStatus.Broadcast(Status,RuntimeStatus);
	else
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: OnIndexerRuntimeStatus]"));
}

void USequenceWalletBP::CallOnIndexerGetChainId(const FSequenceResponseStatus& Status, int64 ChainId) const
{
	if (this->OnIndexerGetChainId.IsBound())
		this->OnIndexerGetChainId.Broadcast(Status,ChainId);
	else
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: OnIndexerGetChainId]"));
}

void USequenceWalletBP::CallOnIndexerGetEtherBalance(const FSequenceResponseStatus& Status, const FEtherBalance& EtherBalance) const
{
	if (this->OnIndexerGetEtherBalance.IsBound())
		this->OnIndexerGetEtherBalance.Broadcast(Status,EtherBalance);
	else
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: OnIndexerGetEtherBalance]"));
}

void USequenceWalletBP::CallOnIndexerGetTokenBalances(const FSequenceResponseStatus& Status, const FGetTokenBalancesReturn& TokenBalances) const
{
	if (this->OnIndexerGetTokenBalances.IsBound())
		this->OnIndexerGetTokenBalances.Broadcast(Status,TokenBalances);
	else
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: OnIndexerGetTokenBalances]"));
}

void USequenceWalletBP::CallOnIndexerGetTokenSupplies(const FSequenceResponseStatus& Status, const FGetTokenSuppliesReturn& TokenSupplies) const
{
	if (this->OnIndexerGetTokenSupplies.IsBound())
		this->OnIndexerGetTokenSupplies.Broadcast(Status,TokenSupplies);
	else
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: OnIndexerGetTokenSupplies]"));
}

void USequenceWalletBP::CallOnIndexerGetTokenSuppliesMap(const FSequenceResponseStatus& Status, const FGetTokenSuppliesMapReturn& TokenSuppliesMap) const
{
	if (this->OnIndexerGetTokenSuppliesMap.IsBound())
		this->OnIndexerGetTokenSuppliesMap.Broadcast(Status,TokenSuppliesMap);
	else
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: OnIndexerGetTokenSuppliesMap]"));
}

void USequenceWalletBP::CallOnIndexerGetBalanceUpdates(const FSequenceResponseStatus& Status, const FGetBalanceUpdatesReturn& BalanceUpdates) const
{
	if (this->OnIndexerGetBalanceUpdates.IsBound())
		this->OnIndexerGetBalanceUpdates.Broadcast(Status,BalanceUpdates);
	else
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: OnIndexerGetBalanceUpdates]"));
}

void USequenceWalletBP::CallOnIndexerGetTransactionHistory(const FSequenceResponseStatus& Status, const FGetTransactionHistoryReturn& TransactionHistory) const
{
	if (this->OnIndexerGetTransactionHistory.IsBound())
		this->OnIndexerGetTransactionHistory.Broadcast(Status,TransactionHistory);
	else
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: OnIndexerGetTransactionHistory]"));
}

float USequenceWalletBP::GetUserReadableAmountIntDecimals(int64 Amount, int32 Decimals)
{
	return UIndexerSupport::GetAmount(Amount,Decimals);
}

float USequenceWalletBP::GetUserReadableAmountFloatDecimals(int64 Amount, float Decimals)
{
	return UIndexerSupport::GetAmount(Amount,Decimals);
}

int64 USequenceWalletBP::GetTransactionReadableAmountIntDecimals(float Amount, int32 Decimals)
{
	return UIndexerSupport::GetAmount(Amount,Decimals);
}

int64 USequenceWalletBP::GetTransactionReadableAmountFloatDecimals(float Amount, float Decimals)
{
	return UIndexerSupport::GetAmount(Amount,Decimals);
}

FString USequenceWalletBP::ApiGetWalletAddress()
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		return Wallet->GetWalletAddress();
	}
	return "";
}

int64 USequenceWalletBP::ApiGetNetworkId()
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		return Wallet->GetNetworkId();
	}
	return 0;
}

void USequenceWalletBP::ApiUpdateNetworkId(int64 NewNetworkId)
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->UpdateNetworkId(NewNetworkId);
	}
}

void USequenceWalletBP::ApiUpdateProviderUrl(const FString& NewProviderUrl)
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->UpdateProviderURL(NewProviderUrl);
	}
}

void USequenceWalletBP::ApiOpenTransakUrl(const FTransakSettings& Settings)
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->OpenTransakLink(Settings.FiatCurrency,Settings.FiatAmount,Settings.CryptoCurrency,Settings.Networks,Settings.DisableWalletAddressForm);
	}
}

void USequenceWalletBP::ApiSignOut()
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->SignOut();
	}
}

void USequenceWalletBP::ApiSignMessage(const FString& Message)
{
	const TFunction<void (FSignedMessage)> OnSuccess = [this](const FSignedMessage& SignedMessage)
	{
		this->CallOnApiSignMessage(FSequenceResponseStatus(true, SignMessageTrt), SignedMessage);
	};

	const TFunction<void (FSequenceError)> OnFailure = [this](const FSequenceError& Err)
	{
		this->CallOnApiSignMessage(FSequenceResponseStatus(false, Err.Message,SignMessageTrt), FSignedMessage());
	};
	
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->SignMessage(Message, OnSuccess, OnFailure);
	}
}

void USequenceWalletBP::ApiGetFilteredFeeOptions(UTransactions * Transactions)
{
	const TFunction<void (TArray<FFeeOption>)> OnSuccess = [this](const TArray<FFeeOption>& FeeOptions)
	{
		this->CallOnApiGetFilteredFeeOptions(FSequenceResponseStatus(true, GetFilteredFeeTrt), FeeOptions);
	};

	const TFunction<void (FSequenceError)> OnFailure = [this](const FSequenceError& Err)
	{
		this->CallOnApiGetFilteredFeeOptions(FSequenceResponseStatus(false, Err.Message, GetFilteredFeeTrt),{});
	};
	
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->GetFeeOptions(Transactions->GetTransactions(), OnSuccess, OnFailure);
	}
}

void USequenceWalletBP::ApiGetUnfilteredFeeOptions(UTransactions * Transactions)
{
	const TFunction<void (TArray<FFeeOption>)> OnSuccess = [this](const TArray<FFeeOption>& FeeOptions)
	{
		this->CallOnApiGetUnFilteredFeeOptions(FSequenceResponseStatus(true, GetUnFilteredFeeOptionsTrt), FeeOptions);
	};

	const TFunction<void (FSequenceError)> OnFailure = [this](const FSequenceError& Err)
	{
		this->CallOnApiGetUnFilteredFeeOptions(FSequenceResponseStatus(false, Err.Message, GetUnFilteredFeeOptionsTrt), {});
	};
	
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->GetUnfilteredFeeOptions(Transactions->GetTransactions(), OnSuccess, OnFailure);
	}
}

void USequenceWalletBP::ApiSendTransactionWithFee(UTransactions * Transactions)
{
	const TFunction<void (FTransactionResponse)> OnSuccess = [this](const FTransactionResponse& Response)
	{
		this->CallOnApiSendTransactionWithFee(FSequenceResponseStatus(true, SendTransactionWithFeeTrt), Response);
	};

	const TFunction<void (FSequenceError)> OnFailure = [this](const FSequenceError& Err)
	{
		this->CallOnApiSendTransactionWithFee(FSequenceResponseStatus(false, Err.Message, SendTransactionWithFeeTrt), FTransactionResponse());
	};
	
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Wallet = WalletOptional.GetValue();
		if (Transactions->IsFeeSet())
		{
			Wallet->SendTransactionWithFeeOption(Transactions->GetTransactions(), Transactions->GetFee(), OnSuccess, OnFailure);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Fee was set for a transaction being sent with a Fee!"));
		}
	}
}

void USequenceWalletBP::ApiSendTransaction(UTransactions * Transactions)
{
	const TFunction<void (FTransactionResponse)> OnSuccess = [this](const FTransactionResponse& Response)
	{
		this->CallOnApiSendTransaction(FSequenceResponseStatus(true, SendTransactionTrt), Response);
	};

	const TFunction<void (FSequenceError)> OnFailure = [this](const FSequenceError& Err)
	{
		this->CallOnApiSendTransaction(FSequenceResponseStatus(false, Err.Message, SendTransactionTrt), FTransactionResponse());
	};
	
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->SendTransaction(Transactions->GetTransactions(), OnSuccess, OnFailure);
	}
}

void USequenceWalletBP::ApiListSessions()
{
	const TFunction<void (TArray<FSession>)> OnSuccess = [this](const TArray<FSession>& Sessions)
	{
		this->CallOnApiListSessions(FSequenceResponseStatus(true, ListSessionsTrt), Sessions);
	};

	const TFunction<void (FSequenceError)> OnFailure = [this](const FSequenceError& Err)
	{
		this->CallOnApiListSessions(FSequenceResponseStatus(false, Err.Message, ListSessionsTrt), {});
	};
	
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->ListSessions(OnSuccess, OnFailure);
	}
}

void USequenceWalletBP::ApiGetSupportedTransakCountries()
{
	const TFunction<void (TArray<FSupportedCountry>)> OnSuccess = [this](const TArray<FSupportedCountry>& SupportedCountries)
	{
		this->CallOnApiGetSupportedTransakCountries(FSequenceResponseStatus(true, GetSupportedTransakCountriesTrt), SupportedCountries);
	};

	const TFunction<void (FSequenceError)> OnFailure = [this](const FSequenceError& Err)
	{
		this->CallOnApiGetSupportedTransakCountries(FSequenceResponseStatus(false, Err.Message, GetSupportedTransakCountriesTrt), {});
	};
	
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->GetSupportedTransakCountries(OnSuccess, OnFailure);
	}
}

void USequenceWalletBP::IndexerPing()
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();

		const TSuccessCallback<bool> OnSuccess = [this](const bool bSuccess)
		{
			this->CallOnIndexerPing(FSequenceResponseStatus(true, PingTrt), bSuccess);
		};

		const FFailureCallback OnFailure = [this](const FSequenceError& Error)
		{
			this->CallOnIndexerPing(FSequenceResponseStatus(false,Error.Message, PingTrt),false);
		};
		
		Wallet->Ping(OnSuccess, OnFailure);
	}
}

void USequenceWalletBP::IndexerVersion()
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();

		const TSuccessCallback<FVersion> OnSuccess = [this](const FVersion& Version)
		{
			this->CallOnIndexerVersion(FSequenceResponseStatus(true, VersionTrt), Version);
		};

		const FFailureCallback OnFailure = [this](const FSequenceError& Error)
		{
			this->CallOnIndexerVersion(FSequenceResponseStatus(false, Error.Message, VersionTrt), FVersion());
		};
		
		Wallet->Version(OnSuccess, OnFailure);
	}
}

void USequenceWalletBP::IndexerRuntimeStatus()
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();

		const TSuccessCallback<FRuntimeStatus> OnSuccess = [this](const FRuntimeStatus& RuntimeStatus)
		{
			this->CallOnIndexerRuntimeStatus(FSequenceResponseStatus(true, RuntimeStatusTrt), RuntimeStatus);
		};

		const FFailureCallback OnFailure = [this](const FSequenceError& Error)
		{
			this->CallOnIndexerRuntimeStatus(FSequenceResponseStatus(false, Error.Message, RuntimeStatusTrt), FRuntimeStatus());
		};
		
		Wallet->RunTimeStatus(OnSuccess, OnFailure);
	}
}

void USequenceWalletBP::IndexerGetChainId()
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();

		const TSuccessCallback<int64> OnSuccess = [this](const int64& ChainId)
		{
			this->CallOnIndexerGetChainId(FSequenceResponseStatus(true, GetChainIdTrt), ChainId);
		};

		const FFailureCallback OnFailure = [this](const FSequenceError& Error)
		{
			this->CallOnIndexerGetChainId(FSequenceResponseStatus(false, Error.Message, GetChainIdTrt),-1);
		};
		
		Wallet->GetChainID(OnSuccess, OnFailure);
	}
}

void USequenceWalletBP::IndexerGetEtherBalance(const FString& WalletAddress)
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();

		const TSuccessCallback<FEtherBalance> OnSuccess = [this](const FEtherBalance& EtherBalance)
		{
			this->CallOnIndexerGetEtherBalance(FSequenceResponseStatus(true, GetEtherBalanceTrt), EtherBalance);
		};

		const FFailureCallback OnFailure = [this](const FSequenceError& Error)
		{
			this->CallOnIndexerGetEtherBalance(FSequenceResponseStatus(false, Error.Message, GetEtherBalanceTrt), FEtherBalance());
		};
		
		Wallet->GetEtherBalance(WalletAddress, OnSuccess, OnFailure);
	}
}

void USequenceWalletBP::IndexerGetTokenBalances(const FGetTokenBalancesArgs& Args)
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();

		const TSuccessCallback<FGetTokenBalancesReturn> OnSuccess = [this](const FGetTokenBalancesReturn& TokenBalances)
		{
			this->CallOnIndexerGetTokenBalances(FSequenceResponseStatus(true, GetTokenBalancesTrt),TokenBalances);
		};

		const FFailureCallback OnFailure = [this](const FSequenceError& Error)
		{
			this->CallOnIndexerGetTokenBalances(FSequenceResponseStatus(false, Error.Message, GetTokenBalancesTrt),FGetTokenBalancesReturn());
		};
		
		Wallet->GetTokenBalances(Args, OnSuccess, OnFailure);
	}
}

void USequenceWalletBP::IndexerGetTokenSupplies(const FGetTokenSuppliesArgs& Args)
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();

		const TSuccessCallback<FGetTokenSuppliesReturn> OnSuccess = [this](const FGetTokenSuppliesReturn& TokenSupplies)
		{
			this->CallOnIndexerGetTokenSupplies(FSequenceResponseStatus(true, GetTokenSuppliesTrt),TokenSupplies);
		};

		const FFailureCallback OnFailure = [this](const FSequenceError& Error)
		{
			this->CallOnIndexerGetTokenSupplies(FSequenceResponseStatus(false, Error.Message, GetTokenSuppliesTrt), FGetTokenSuppliesReturn());
		};
		
		Wallet->GetTokenSupplies(Args, OnSuccess, OnFailure);
	}
}

void USequenceWalletBP::IndexerGetTokenSuppliesMap(const FGetTokenSuppliesMapArgs& Args)
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();

		const TSuccessCallback<FGetTokenSuppliesMapReturn> OnSuccess = [this](const FGetTokenSuppliesMapReturn& TokenSuppliesMap)
		{
			this->CallOnIndexerGetTokenSuppliesMap(FSequenceResponseStatus(true, GetTokenSuppliesMapTrt), TokenSuppliesMap);
		};

		const FFailureCallback OnFailure = [this](const FSequenceError& Error)
		{
			this->CallOnIndexerGetTokenSuppliesMap(FSequenceResponseStatus(false, Error.Message, GetTokenSuppliesMapTrt),FGetTokenSuppliesMapReturn());
		};
		
		Wallet->GetTokenSuppliesMap(Args, OnSuccess, OnFailure);
	}
}

void USequenceWalletBP::IndexerGetBalanceUpdates(const FGetBalanceUpdatesArgs& Args)
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();

		const TSuccessCallback<FGetBalanceUpdatesReturn> OnSuccess = [this](const FGetBalanceUpdatesReturn& BalanceUpdates)
		{
			this->CallOnIndexerGetBalanceUpdates(FSequenceResponseStatus(true, GetBalanceUpdatesTrt), BalanceUpdates);
		};

		const FFailureCallback OnFailure = [this](const FSequenceError& Error)
		{
			this->CallOnIndexerGetBalanceUpdates(FSequenceResponseStatus(false, Error.Message, GetBalanceUpdatesTrt), FGetBalanceUpdatesReturn());
		};
		
		Wallet->GetBalanceUpdates(Args, OnSuccess, OnFailure);
	}
}

void USequenceWalletBP::IndexerGetTransactionHistory(const FGetTransactionHistoryArgs& Args)
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();

		const TSuccessCallback<FGetTransactionHistoryReturn> OnSuccess = [this](const FGetTransactionHistoryReturn& TransactionHistory)
		{
			this->CallOnIndexerGetTransactionHistory(FSequenceResponseStatus(true, GetTransactionHistoryTrt), TransactionHistory);
		};

		const FFailureCallback OnFailure = [this](const FSequenceError& Error)
		{
			this->CallOnIndexerGetTransactionHistory(FSequenceResponseStatus(false, Error.Message, GetTransactionHistoryTrt), FGetTransactionHistoryReturn());
		};
		
		Wallet->GetTransactionHistory(Args, OnSuccess, OnFailure);
	}
}
