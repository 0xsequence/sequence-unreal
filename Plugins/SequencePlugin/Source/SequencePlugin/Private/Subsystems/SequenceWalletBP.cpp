// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Subsystems/SequenceWalletBP.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Sequence/SequenceAPI.h"
#include "Util/Log.h"

USequenceWalletBP::USequenceWalletBP() { }

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

void USequenceWalletBP::UpdateNetworkIdFromName(FString NewNetworkName)
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet* Wallet = WalletOptional.GetValue();
		Wallet->UpdateNetworkId(NewNetworkName);
	}
}

void USequenceWalletBP::UpdateNetworkIdFromEnum(ENetwork NewNetwork)
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet* Wallet = WalletOptional.GetValue();
		Wallet->UpdateNetworkId(NewNetwork);
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

void USequenceWalletBP::SignOut()
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->SignOut();
	}
}

void USequenceWalletBP::SignMessage(const FString& Message, FOnSignMessage OnSuccess, FOnFailure OnFailure)
{
	const TFunction<void (FSeqSignMessageResponse_Response)> OnApiSuccess = [this, OnSuccess](const FSeqSignMessageResponse_Response& SignedMessage)
	{
		OnSuccess.Execute(FSequenceResponseStatus(true, SignMessageTrt), SignedMessage);
	};

	const TFunction<void (FSequenceError)> OnApiFailure = [this, OnFailure](const FSequenceError& Err)
	{
		OnFailure.Execute(Err.Message);
	};
	
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->SignMessage(Message, OnApiSuccess, OnApiFailure);
	}
}

void USequenceWalletBP::ValidateMessageSignature(const int64& ChainId, const FString& WalletAddress, const FString& Message, const FString& Signature, FOnValidateMessageSignature OnSuccess, FOnFailure OnFailure)
{
	const TFunction<void(FSeqValidateMessageSignatureResponse_Data)> OnApiSuccess = [this, OnSuccess](const FSeqValidateMessageSignatureResponse_Data& isValidMessageSignature)
		{
			OnSuccess.Execute(FSequenceResponseStatus(true, ValidateMessageSignatureTrt), isValidMessageSignature);
		};

	const TFunction<void(FSequenceError)> OnApiFailure = [this, OnFailure](const FSequenceError& Err)
	{
		OnFailure.Execute(Err.Message);
	};

	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet* Wallet = WalletOptional.GetValue();
		Wallet->ValidateMessageSignature(ChainId, WalletAddress, Message, Signature, OnApiSuccess, OnApiFailure);
	}
}

void USequenceWalletBP::GetFilteredFeeOptions(UTransactions * Transactions, FOnGetFilteredFeeOptions OnSuccess, FOnFailure OnFailure)
{
	const TFunction<void (TArray<FFeeOption>)> OnApiSuccess = [this, OnSuccess](const TArray<FFeeOption>& FeeOptions)
	{
		OnSuccess.Execute(FSequenceResponseStatus(true, GetFilteredFeeTrt), FeeOptions);
	};

	const TFunction<void (FSequenceError)> OnApiFailure = [this, OnFailure](const FSequenceError& Err)
	{
		OnFailure.Execute(Err.Message);
	};
	
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->GetFeeOptions(Transactions->GetTransactions(), OnApiSuccess, OnApiFailure);
	}
}

void USequenceWalletBP::GetUnfilteredFeeOptions(UTransactions * Transactions, FOnGetUnFilteredFeeOptions OnSuccess, FOnFailure OnFailure)
{
	const TFunction<void (TArray<FFeeOption>)> OnApiSuccess = [this, OnSuccess](const TArray<FFeeOption>& FeeOptions)
	{
		OnSuccess.Execute(FSequenceResponseStatus(true, GetUnFilteredFeeOptionsTrt), FeeOptions);
	};

	const TFunction<void (FSequenceError)> OnApiFailure = [this, OnFailure](const FSequenceError& Err)
	{
		OnFailure.Execute(Err.Message);
	};
	
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->GetUnfilteredFeeOptions(Transactions->GetTransactions(), OnApiSuccess, OnApiFailure);
	}
}

void USequenceWalletBP::SendEther(const FString& RecipientAddress, const FString& Amount, FOnSendTransaction OnSuccess, FOnFailure OnFailure)
{
	const TFunction<void (FSeqTransactionResponse_Data)> OnApiSuccess = [this, OnSuccess](const FSeqTransactionResponse_Data& Response)
	{
		OnSuccess.Execute(FSequenceResponseStatus(true, SendTransactionWithFeeTrt), Response);
	};

	const TFunction<void (FSequenceError)> OnApiFailure = [this, OnFailure](const FSequenceError& Err)
	{
		OnFailure.Execute(Err.Message);
	};
	
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->SendEther(RecipientAddress, Amount, OnApiSuccess, OnApiFailure);
	}
}

void USequenceWalletBP::SendTransactionWithFee(UTransactions * Transactions, FOnSendTransactionWtihFeeOption OnSuccess, FOnFailure OnFailure)
{
	const TFunction<void (FSeqTransactionResponse_Data)> OnApiSuccess = [this, OnSuccess](const FSeqTransactionResponse_Data& Response)
	{
		OnSuccess.Execute(FSequenceResponseStatus(true, SendTransactionWithFeeTrt), Response);
	};

	const TFunction<void (FSequenceError)> OnApiFailure = [this, OnFailure](const FSequenceError& Err)
	{
		OnFailure.Execute(Err.Message);
	};
	
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		if (Transactions->IsFeeSet())
		{
			Wallet->SendTransactionWithFeeOption(Transactions->GetTransactions(), Transactions->GetFee(), OnApiSuccess, OnApiFailure);
		}
		else
		{
			SEQ_LOG(Warning, TEXT("No Fee was set for a transaction being sent with a Fee!"));
		}
	}
}

void USequenceWalletBP::SendTransaction(UTransactions * Transactions, FOnSendTransaction OnSuccess, FOnFailure OnFailure)
{
	const TFunction<void (FSeqTransactionResponse_Data)> OnApiSuccess = [this, OnSuccess](const FSeqTransactionResponse_Data& Response)
	{
		OnSuccess.Execute(FSequenceResponseStatus(true, SendTransactionTrt), Response);
	};

	const TFunction<void (FSequenceError)> OnApiFailure = [this, OnFailure](const FSequenceError& Err)
	{
		OnFailure.Execute(Err.Message);
	};
	
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->SendTransaction(Transactions->GetTransactions(), OnApiSuccess, OnApiFailure);
	}
}

void USequenceWalletBP::GetIdToken(const FString& Nonce, FOnGetIdToken OnSuccess, FOnFailure OnFailure)
{
	const TFunction<void (FSeqIdTokenResponse_Data)> OnApiSuccess = [this, OnSuccess](const FSeqIdTokenResponse_Data& Data)
		{
			OnSuccess.Execute(FSequenceResponseStatus(true, GetIdTokenTrt), Data);
		};

	const TFunction<void(FSequenceError)> OnApiFailure = [this, OnFailure](const FSequenceError& Err)
	{
		OnFailure.Execute(Err.Message);
	};

	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet* Wallet = WalletOptional.GetValue();
		Wallet->GetIdToken(Nonce, OnApiSuccess, OnApiFailure);
	}
}

void USequenceWalletBP::ListSessions(FOnListSessions OnSuccess, FOnFailure OnFailure)
{
	const TFunction<void (TArray<FSeqListSessions_Session>)> OnApiSuccess = [this, OnSuccess](const TArray<FSeqListSessions_Session>& Sessions)
	{
		OnSuccess.Execute(FSequenceResponseStatus(true, ListSessionsTrt), Sessions);
	};

	const TFunction<void (FSequenceError)> OnApiFailure = [this, OnFailure](const FSequenceError& Err)
	{
		OnFailure.Execute(Err.Message);
	};
	
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->ListSessions(OnApiSuccess, OnApiFailure);
	}
}

void USequenceWalletBP::ListAccounts(FOnListAccounts OnSuccess, FOnFailure OnFailure)
{
	const TFunction<void(FSeqListAccountsResponse_Data)> OnApiSuccess = [this, OnSuccess](const FSeqListAccountsResponse_Data& Response)
	{
		OnSuccess.Execute(FSequenceResponseStatus(true, ListAccountsTrt), Response);
	};

	const TFunction<void(FSequenceError)> OnApiFailure = [this, OnFailure](const FSequenceError& Err)
	{
		OnFailure.Execute(Err.Message);
	};

	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet* Wallet = WalletOptional.GetValue();
		Wallet->ListAccounts(OnApiSuccess, OnApiFailure);
	}
}

void USequenceWalletBP::GetSessionAuthProof(const FString& Nonce, FOnGetSessionAuthProof OnSuccess, FOnFailure OnFailure)
{
	const TFunction<void(FSeqGetSessionAuthProof_Data)> OnApiSuccess = [this, OnSuccess](const FSeqGetSessionAuthProof_Data& Response)
	{
		OnSuccess.Execute(FSequenceResponseStatus(true, GetSessionAuthProofTrt), Response);
	};

	const TFunction<void(FSequenceError)> OnApiFailure = [this, OnFailure](const FSequenceError& Err)
	{
		OnFailure.Execute(Err.Message);
	};

	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet* Wallet = WalletOptional.GetValue();
		Wallet->GetSessionAuthProof(Nonce,OnApiSuccess, OnApiFailure);
	}
}
