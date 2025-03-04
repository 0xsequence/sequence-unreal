// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Subsystems/SequenceWalletBP.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Sequence/SequenceAPI.h"
#include "Util/Log.h"

USequenceWalletBP::USequenceWalletBP() { }

void USequenceWalletBP::CallOnApiSignMessage(const FSequenceResponseStatus& Status, const FSeqSignMessageResponse_Response& SignedMessage) const
{
	if (this->OnApiSignMessage.IsBound())
		this->OnApiSignMessage.Broadcast(Status,SignedMessage);
	else
		SEQ_LOG(Error, TEXT("[Nothing bound to: OnApiSignMessage]"));
}

void USequenceWalletBP::CallOnApiValidateMessageSignature(const FSequenceResponseStatus& Status, const FSeqValidateMessageSignatureResponse_Data& isValidMessageSignature) const
{
	if (this->OnApiValidateMessageSignature.IsBound())
		this->OnApiValidateMessageSignature.Broadcast(Status, isValidMessageSignature);
	else
		SEQ_LOG(Error, TEXT("[Nothing bound to: OnApiIsValidMessageSignature]"));
}

void USequenceWalletBP::CallOnApiGetFilteredFeeOptions(const FSequenceResponseStatus& Status, const TArray<FFeeOption>& FeeOptions) const
{
	if (this->OnApiGetFilteredFeeOptions.IsBound())
		this->OnApiGetFilteredFeeOptions.Broadcast(Status,FeeOptions);
	else
		SEQ_LOG(Error, TEXT("[Nothing bound to: OnApiGetFilteredFeeOptions]"));
}

void USequenceWalletBP::CallOnApiGetUnFilteredFeeOptions(const FSequenceResponseStatus& Status, const TArray<FFeeOption>& FeeOptions) const
{
	if (this->OnApiGetUnFilteredFeeOptions.IsBound())
		this->OnApiGetUnFilteredFeeOptions.Broadcast(Status,FeeOptions);
	else
		SEQ_LOG(Error, TEXT("[Nothing bound to: OnApiGetUnFilteredFeeOptions]"));
}

void USequenceWalletBP::CallOnApiSendEther(const FSequenceResponseStatus& Status, const FSeqTransactionResponse_Data& Response) const
{
	if (this->OnApiSendEther.IsBound())
		this->OnApiSendEther.Broadcast(Status, Response);
	else
		SEQ_LOG(Error, TEXT("[Nothing bound to: OnApiSendEther]"));
}

void USequenceWalletBP::CallOnApiSendTransactionWithFee(const FSequenceResponseStatus& Status, const FSeqTransactionResponse_Data& Response) const
{
	if (this->OnApiSendTransactionWithFeeOption.IsBound())
		this->OnApiSendTransactionWithFeeOption.Broadcast(Status,Response);
	else
		SEQ_LOG(Error, TEXT("[Nothing bound to: OnApiSendTransactionWithFeeOption]"));
}

void USequenceWalletBP::CallOnApiSendTransaction(const FSequenceResponseStatus& Status, const FSeqTransactionResponse_Data& Response) const
{
	if (this->OnApiSendTransaction.IsBound())
		this->OnApiSendTransaction.Broadcast(Status,Response);
	else
		SEQ_LOG(Error, TEXT("[Nothing bound to: OnApiSendTransaction]"));
}

void USequenceWalletBP::CallOnApiGetIdToken(const FSequenceResponseStatus& Status, const FSeqIdTokenResponse_Data& Response) const
{
	if (this->OnApiGetIdToken.IsBound())
		this->OnApiGetIdToken.Broadcast(Status, Response);
	else
		SEQ_LOG(Error, TEXT("[Nothing bound to: OnApiGetIdToken]"));

}

void USequenceWalletBP::CallOnApiListSessions(const FSequenceResponseStatus& Status, const TArray<FSeqListSessions_Session>& Sessions) const
{
	if (this->OnApiListSessions.IsBound())
		this->OnApiListSessions.Broadcast(Status,Sessions);
	else
		SEQ_LOG(Error, TEXT("[Nothing bound to: OnApiListSessions]"));
}

void USequenceWalletBP::CallOnApiListAccounts(const FSequenceResponseStatus& Status, const FSeqListAccountsResponse_Data& Response) const
{
	if (this->OnApiListAccounts.IsBound())
		this->OnApiListAccounts.Broadcast(Status, Response);
	else
		SEQ_LOG(Error, TEXT("[Nothing bound to: OnApiListAccounts]"));
}

void USequenceWalletBP::CallOnApiGetSessionAuthProof(const FSequenceResponseStatus& Status, const FSeqGetSessionAuthProof_Data Response) const
{
	if (this->OnApiGetSessionAuthProof.IsBound())
		this->OnApiGetSessionAuthProof.Broadcast(Status, Response);
	else
		SEQ_LOG(Error, TEXT("[Nothing bound to: OnApiGetSessionAuthProof]"));
}

void USequenceWalletBP::CallOnApiGetSupportedTransakCountries(const FSequenceResponseStatus& Status, const TArray<FSupportedCountry>& SupportedCountries) const
{
	if (this->OnApiGetSupportedTransakCountries.IsBound())
		this->OnApiGetSupportedTransakCountries.Broadcast(Status,SupportedCountries);
	else
		SEQ_LOG(Error, TEXT("[Nothing bound to: OnApiGetSupportedTransakCountries]"));
}

float USequenceWalletBP::GetUserReadableAmountIntDecimals(int64 Amount, int64 Decimals)
{
	return USequenceSupport::GetUserReadableAmount(Amount,Decimals);
}

int64 USequenceWalletBP::GetTransactionReadableAmountIntDecimals(float Amount, int64 Decimals)
{
	return USequenceSupport::GetSystemReadableAmount(Amount,Decimals);
}

int64 USequenceWalletBP::GetNetworkIdFromName(const FString& NetworkNameIn)
{
	return USequenceSupport::GetNetworkId(NetworkNameIn);
}

int64 USequenceWalletBP::GetNetworkIdFromNetworkEnum(const ENetwork& NetworkEnumIn)
{
	return USequenceSupport::GetNetworkId(NetworkEnumIn);
}

FString USequenceWalletBP::GetNetworkNameFromId(const int64 NetworkIdIn)
{
	return USequenceSupport::GetNetworkName(NetworkIdIn);
}

FString USequenceWalletBP::GetNetworkNameFromEnum(const ENetwork NetworkIn)
{
	return USequenceSupport::GetNetworkName(NetworkIn);
}

TArray<FIdNamePair> USequenceWalletBP::GetAllNetworks()
{
	return USequenceSupport::GetAllNetworks();
}

TArray<FString> USequenceWalletBP::GetAllNetworkNames()
{
	return USequenceSupport::GetAllNetworkNames();
}

TArray<int64> USequenceWalletBP::GetAllNetworkIds()
{
	return USequenceSupport::GetAllNetworkIds();
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

void USequenceWalletBP::ApiUpdateNetworkIdFromName(FString NewNetworkName)
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet* Wallet = WalletOptional.GetValue();
		Wallet->UpdateNetworkId(NewNetworkName);
	}
}

void USequenceWalletBP::ApiUpdateNetworkIdFromEnum(ENetwork NewNetwork)
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet* Wallet = WalletOptional.GetValue();
		Wallet->UpdateNetworkId(NewNetwork);
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
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->SignOut();
	}
}

void USequenceWalletBP::ApiSignMessage(const FString& Message)
{
	const TFunction<void (FSeqSignMessageResponse_Response)> OnSuccess = [this](const FSeqSignMessageResponse_Response& SignedMessage)
	{
		this->CallOnApiSignMessage(FSequenceResponseStatus(true, SignMessageTrt), SignedMessage);
	};

	const TFunction<void (FSequenceError)> OnFailure = [this](const FSequenceError& Err)
	{
		this->CallOnApiSignMessage(FSequenceResponseStatus(false, Err.Message,SignMessageTrt), FSeqSignMessageResponse_Response());
	};
	
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->SignMessage(Message, OnSuccess, OnFailure);
	}
}

void USequenceWalletBP::ApiValidateMessageSignature(const int64& ChainId, const FString& WalletAddress, const FString& Message, const FString& Signature)
{
	const TFunction<void(FSeqValidateMessageSignatureResponse_Data)> OnSuccess = [this](const FSeqValidateMessageSignatureResponse_Data& isValidMessageSignature)
		{
			this->CallOnApiValidateMessageSignature(FSequenceResponseStatus(true, ValidateMessageSignatureTrt), isValidMessageSignature);
		};

	const TFunction<void(FSequenceError)> OnFailure = [this](const FSequenceError& Err)
		{
			this->CallOnApiValidateMessageSignature(FSequenceResponseStatus(false, Err.Message, ValidateMessageSignatureTrt), FSeqValidateMessageSignatureResponse_Data());
		};

	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet* Wallet = WalletOptional.GetValue();
		Wallet->ValidateMessageSignature(ChainId, WalletAddress, Message, Signature, OnSuccess, OnFailure);
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
		const USequenceWallet * Wallet = WalletOptional.GetValue();
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
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->GetUnfilteredFeeOptions(Transactions->GetTransactions(), OnSuccess, OnFailure);
	}
}

void USequenceWalletBP::ApiSendEther(const FString& RecipientAddress, const FString& Amount)
{
	const TFunction<void (FSeqTransactionResponse_Data)> OnSuccess = [this](const FSeqTransactionResponse_Data& Response)
	{
		this->CallOnApiSendEther(FSequenceResponseStatus(true, SendTransactionWithFeeTrt), Response);
	};

	const TFunction<void (FSequenceError)> OnFailure = [this](const FSequenceError& Err)
	{
		this->CallOnApiSendEther(FSequenceResponseStatus(false, Err.Message, SendTransactionWithFeeTrt), FSeqTransactionResponse_Data());
	};
	
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->SendEther(RecipientAddress, Amount, OnSuccess, OnFailure);
	}
}

void USequenceWalletBP::ApiSendTransactionWithFee(UTransactions * Transactions)
{
	const TFunction<void (FSeqTransactionResponse_Data)> OnSuccess = [this](const FSeqTransactionResponse_Data& Response)
	{
		this->CallOnApiSendTransactionWithFee(FSequenceResponseStatus(true, SendTransactionWithFeeTrt), Response);
	};

	const TFunction<void (FSequenceError)> OnFailure = [this](const FSequenceError& Err)
	{
		this->CallOnApiSendTransactionWithFee(FSequenceResponseStatus(false, Err.Message, SendTransactionWithFeeTrt), FSeqTransactionResponse_Data());
	};
	
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		if (Transactions->IsFeeSet())
		{
			Wallet->SendTransactionWithFeeOption(Transactions->GetTransactions(), Transactions->GetFee(), OnSuccess, OnFailure);
		}
		else
		{
			SEQ_LOG(Warning, TEXT("No Fee was set for a transaction being sent with a Fee!"));
		}
	}
}

void USequenceWalletBP::ApiSendTransaction(UTransactions * Transactions)
{
	const TFunction<void (FSeqTransactionResponse_Data)> OnSuccess = [this](const FSeqTransactionResponse_Data& Response)
	{
		this->CallOnApiSendTransaction(FSequenceResponseStatus(true, SendTransactionTrt), Response);
	};

	const TFunction<void (FSequenceError)> OnFailure = [this](const FSequenceError& Err)
	{
		this->CallOnApiSendTransaction(FSequenceResponseStatus(false, Err.Message, SendTransactionTrt), FSeqTransactionResponse_Data());
	};
	
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->SendTransaction(Transactions->GetTransactions(), OnSuccess, OnFailure);
	}
}

void USequenceWalletBP::ApiGetIdToken(const FString& Nonce)
{
	const TFunction<void (FSeqIdTokenResponse_Data)> OnSuccess = [this](const FSeqIdTokenResponse_Data& Data)
		{
			this->CallOnApiGetIdToken(FSequenceResponseStatus(true, GetIdTokenTrt), Data);
		};

	const TFunction<void(FSequenceError)> OnFailure = [this](const FSequenceError& Err)
		{
			this->CallOnApiGetIdToken(FSequenceResponseStatus(false, Err.Message, GetIdTokenTrt), {});
		};

	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet* Wallet = WalletOptional.GetValue();
		Wallet->GetIdToken(Nonce, OnSuccess, OnFailure);
	}
}

void USequenceWalletBP::ApiListSessions()
{
	const TFunction<void (TArray<FSeqListSessions_Session>)> OnSuccess = [this](const TArray<FSeqListSessions_Session>& Sessions)
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
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->ListSessions(OnSuccess, OnFailure);
	}
}

void USequenceWalletBP::ApiListAccounts()
{
	const TFunction<void(FSeqListAccountsResponse_Data)> OnSuccess = [this](const FSeqListAccountsResponse_Data& Response)
		{
			this->CallOnApiListAccounts(FSequenceResponseStatus(true, ListAccountsTrt), Response);
		};

	const TFunction<void(FSequenceError)> OnFailure = [this](const FSequenceError& Err)
		{
			this->CallOnApiListAccounts(FSequenceResponseStatus(false, Err.Message, ListAccountsTrt), {});
		};

	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet* Wallet = WalletOptional.GetValue();
		Wallet->ListAccounts(OnSuccess, OnFailure);
	}
}

void USequenceWalletBP::ApiGetSessionAuthProof(const FString& Nonce)
{

	const TFunction<void(FSeqGetSessionAuthProof_Data)> OnSuccess = [this](const FSeqGetSessionAuthProof_Data& Response)
		{
			this->CallOnApiGetSessionAuthProof(FSequenceResponseStatus(true, GetSessionAuthProofTrt), Response);
		};

	const TFunction<void(FSequenceError)> OnFailure = [this](const FSequenceError& Err)
		{
			this->CallOnApiGetSessionAuthProof(FSequenceResponseStatus(false, Err.Message, GetSessionAuthProofTrt), {});
		};

	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet* Wallet = WalletOptional.GetValue();
		Wallet->GetSessionAuthProof(Nonce,OnSuccess, OnFailure);
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
