// Fill out your copyright notice in the Description page of Project Settings.


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
