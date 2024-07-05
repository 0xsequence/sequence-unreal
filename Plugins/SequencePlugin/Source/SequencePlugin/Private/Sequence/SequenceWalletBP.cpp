// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Sequence/SequenceWalletBP.h"
#include "Sequence/SequenceAPI.h"

USequenceWalletBP::USequenceWalletBP()
{
	
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

FString USequenceWalletBP::GetWalletAddress()
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		return Wallet->GetWalletAddress();
	}
	return "";
}

int64 USequenceWalletBP::GetNetworkId()
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		return Wallet->GetNetworkId();
	}
	return 0;
}

void USequenceWalletBP::UpdateNetworkId(int64 NewNetworkId)
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->UpdateNetworkId(NewNetworkId);
	}
}

void USequenceWalletBP::UpdateProviderUrl(const FString& NewProviderUrl)
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->UpdateProviderURL(NewProviderUrl);
	}
}

void USequenceWalletBP::OpenTransakUrl(const FTransakSettings& Settings)
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->OpenTransakLink(Settings.FiatCurrency,Settings.FiatAmount,Settings.CryptoCurrency,Settings.Networks,Settings.DisableWalletAddressForm);
	}
}

void USequenceWalletBP::SignOut()
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->SignOut();
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
			this->CallOnIndexerPing(FSequenceResponseStatus(true), bSuccess);
		};

		const FFailureCallback OnFailure = [this](const FSequenceError& Error)
		{
			this->CallOnIndexerPing(FSequenceResponseStatus(false,Error.Message),false);
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
			this->CallOnIndexerVersion(FSequenceResponseStatus(true), Version);
		};

		const FFailureCallback OnFailure = [this](const FSequenceError& Error)
		{
			this->CallOnIndexerVersion(FSequenceResponseStatus(false, Error.Message), FVersion());
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
			this->CallOnIndexerRuntimeStatus(FSequenceResponseStatus(true), RuntimeStatus);
		};

		const FFailureCallback OnFailure = [this](const FSequenceError& Error)
		{
			this->CallOnIndexerRuntimeStatus(FSequenceResponseStatus(false, Error.Message), FRuntimeStatus());
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
			this->CallOnIndexerGetChainId(FSequenceResponseStatus(true), ChainId);
		};

		const FFailureCallback OnFailure = [this](const FSequenceError& Error)
		{
			this->CallOnIndexerGetChainId(FSequenceResponseStatus(false, Error.Message),-1);
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
			this->CallOnIndexerGetEtherBalance(FSequenceResponseStatus(true), EtherBalance);
		};

		const FFailureCallback OnFailure = [this](const FSequenceError& Error)
		{
			this->CallOnIndexerGetEtherBalance(FSequenceResponseStatus(false, Error.Message), FEtherBalance());
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
			this->CallOnIndexerGetTokenBalances(FSequenceResponseStatus(true),TokenBalances);
		};

		const FFailureCallback OnFailure = [this](const FSequenceError& Error)
		{
			this->CallOnIndexerGetTokenBalances(FSequenceResponseStatus(false, Error.Message),FGetTokenBalancesReturn());
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
			this->CallOnIndexerGetTokenSupplies(FSequenceResponseStatus(true),TokenSupplies);
		};

		const FFailureCallback OnFailure = [this](const FSequenceError& Error)
		{
			this->CallOnIndexerGetTokenSupplies(FSequenceResponseStatus(false, Error.Message), FGetTokenSuppliesReturn());
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
			this->CallOnIndexerGetTokenSuppliesMap(FSequenceResponseStatus(true), TokenSuppliesMap);
		};

		const FFailureCallback OnFailure = [this](const FSequenceError& Error)
		{
			this->CallOnIndexerGetTokenSuppliesMap(FSequenceResponseStatus(false, Error.Message),FGetTokenSuppliesMapReturn());
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
			this->CallOnIndexerGetBalanceUpdates(FSequenceResponseStatus(true), BalanceUpdates);
		};

		const FFailureCallback OnFailure = [this](const FSequenceError& Error)
		{
			this->CallOnIndexerGetBalanceUpdates(FSequenceResponseStatus(false, Error.Message), FGetBalanceUpdatesReturn());
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
			this->CallOnIndexerGetTransactionHistory(FSequenceResponseStatus(true), TransactionHistory);
		};

		const FFailureCallback OnFailure = [this](const FSequenceError& Error)
		{
			this->CallOnIndexerGetTransactionHistory(FSequenceResponseStatus(false, Error.Message), FGetTransactionHistoryReturn());
		};
		
		Wallet->GetTransactionHistory(Args, OnSuccess, OnFailure);
	}
}
