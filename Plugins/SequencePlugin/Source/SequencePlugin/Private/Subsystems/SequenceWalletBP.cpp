// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Subsystems/SequenceWalletBP.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Sequence/SequenceWallet.h"
#include "Util/Log.h"

USequenceWalletBP::USequenceWalletBP() { }

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
		this->CallOnSessionClosed();
	}
}

void USequenceWalletBP::SignMessage(const FString& Message, FOnSignMessage OnSuccess, FOnFailure OnFailure)
{
	const TFunction<void (FSeqSignMessageResponse_Response)> OnApiSuccess = [this, OnSuccess](const FSeqSignMessageResponse_Response& SignedMessage)
	{
		OnSuccess.ExecuteIfBound(SignedMessage.Data.Signature);
	};

	const TFunction<void (FSequenceError)> OnApiFailure = [this, OnFailure](const FSequenceError& Err)
	{
		OnFailure.ExecuteIfBound(Err.Message);
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
			OnSuccess.ExecuteIfBound(isValidMessageSignature.isValid);
		};

	const TFunction<void(FSequenceError)> OnApiFailure = [this, OnFailure](const FSequenceError& Err)
	{
		OnFailure.ExecuteIfBound(Err.Message);
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
		OnSuccess.ExecuteIfBound(FeeOptions);
	};

	const TFunction<void (FSequenceError)> OnApiFailure = [this, OnFailure](const FSequenceError& Err)
	{
		OnFailure.ExecuteIfBound(Err.Message);
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
		OnSuccess.ExecuteIfBound(FeeOptions);
	};

	const TFunction<void (FSequenceError)> OnApiFailure = [this, OnFailure](const FSequenceError& Err)
	{
		OnFailure.ExecuteIfBound(Err.Message);
	};
	
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->GetUnfilteredFeeOptions(Transactions->GetTransactions(), OnApiSuccess, OnApiFailure);
	}
}

void USequenceWalletBP::SendNativeToken(const FString& RecipientAddress, const FString& Amount, FOnSendTransaction OnSuccess, FOnFailure OnFailure)
{
	const TFunction<void (FSeqTransactionResponse_Data)> OnApiSuccess = [this, OnSuccess](const FSeqTransactionResponse_Data& Response)
	{
		OnSuccess.ExecuteIfBound(Response.Receipt.TxnReceipt, Response);
	};

	const TFunction<void (FSequenceError)> OnApiFailure = [this, OnFailure](const FSequenceError& Err)
	{
		OnFailure.ExecuteIfBound(Err.Message);
	};
	
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();	
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->SendNativeToken(RecipientAddress, Amount, OnApiSuccess, OnApiFailure);
	}
}

void USequenceWalletBP::SendTransactionWithFee(UTransactions * Transactions, FOnSendTransaction OnSuccess, FOnFailure OnFailure)
{
	const TFunction<void (FSeqTransactionResponse_Data)> OnApiSuccess = [this, OnSuccess](const FSeqTransactionResponse_Data& Response)
	{
		OnSuccess.ExecuteIfBound(Response.Receipt.TxnReceipt, Response);
	};

	const TFunction<void (FSequenceError)> OnApiFailure = [this, OnFailure](const FSequenceError& Err)
	{
		OnFailure.ExecuteIfBound(Err.Message);
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
		OnSuccess.ExecuteIfBound(Response.Receipt.TxnReceipt, Response);
	};

	const TFunction<void (FSequenceError)> OnApiFailure = [this, OnFailure](const FSequenceError& Err)
	{
		OnFailure.ExecuteIfBound(Err.Message);
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
		OnSuccess.ExecuteIfBound(Data.IdToken, Data.ExpiresIn);
	};

	const TFunction<void(FSequenceError)> OnApiFailure = [this, OnFailure](const FSequenceError& Err)
	{
		OnFailure.ExecuteIfBound(Err.Message);
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
		OnSuccess.ExecuteIfBound(Sessions);
	};

	const TFunction<void (FSequenceError)> OnApiFailure = [this, OnFailure](const FSequenceError& Err)
	{
		OnFailure.ExecuteIfBound(Err.Message);
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
		OnSuccess.ExecuteIfBound(Response.Accounts, Response.CurrentAccountId);
	};

	const TFunction<void(FSequenceError)> OnApiFailure = [this, OnFailure](const FSequenceError& Err)
	{
		OnFailure.ExecuteIfBound(Err.Message);
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
		OnSuccess.ExecuteIfBound(Response);
	};

	const TFunction<void(FSequenceError)> OnApiFailure = [this, OnFailure](const FSequenceError& Err)
	{
		OnFailure.ExecuteIfBound(Err.Message);
	};

	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet* Wallet = WalletOptional.GetValue();
		Wallet->GetSessionAuthProof(Nonce,OnApiSuccess, OnApiFailure);
	}
}

void USequenceWalletBP::CallOnSessionClosed() const
{
	if (this->OnSessionClosed.IsBound())
	{
		this->OnSessionClosed.Broadcast();
	}
}

