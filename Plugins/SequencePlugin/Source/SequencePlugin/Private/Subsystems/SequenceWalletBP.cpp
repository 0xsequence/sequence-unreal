// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Subsystems/SequenceWalletBP.h"
#include "Engine/Engine.h"
#include "Sequence/SequenceWallet.h"

USequenceWalletBP::USequenceWalletBP()
{
	this->Wallet = NewObject<USequenceWallet>();
}

FString USequenceWalletBP::GetWalletAddress()
{
	return this->Wallet->GetWalletAddress();
}

void USequenceWalletBP::UpdateProviderUrl(const FString& NewProviderUrl)
{
	return this->Wallet->UpdateProviderURL(NewProviderUrl);
}

void USequenceWalletBP::SignOut()
{
	this->Wallet->SignOut();
	this->CallOnSessionClosed();
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
	
	this->Wallet->SignMessage(Message, OnApiSuccess, OnApiFailure);
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

	this->Wallet->ValidateMessageSignature(ChainId, WalletAddress, Message, Signature, OnApiSuccess, OnApiFailure);
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
	
	this->Wallet->GetFeeOptions(Transactions->GetTransactions(), OnApiSuccess, OnApiFailure);
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
	
	this->Wallet->GetUnfilteredFeeOptions(Transactions->GetTransactions(), OnApiSuccess, OnApiFailure);
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
	
	this->Wallet->SendNativeToken(RecipientAddress, Amount, OnApiSuccess, OnApiFailure);
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
	
	if (Transactions->IsFeeSet())
	{
		this->Wallet->SendTransactionWithFeeOption(Transactions->GetTransactions(), Transactions->GetFee(), OnApiSuccess, OnApiFailure);
	}
	else
	{
		OnFailure.ExecuteIfBound("No Fee was set for a transaction being sent with a Fee!");
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
	
	this->Wallet->SendTransaction(Transactions->GetTransactions(), OnApiSuccess, OnApiFailure);
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

	this->Wallet->GetIdToken(Nonce, OnApiSuccess, OnApiFailure);
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
	
	this->Wallet->ListSessions(OnApiSuccess, OnApiFailure);
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

	this->Wallet->ListAccounts(OnApiSuccess, OnApiFailure);
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

	this->Wallet->GetSessionAuthProof(Nonce,OnApiSuccess, OnApiFailure);
}

void USequenceWalletBP::GetLinkedWallets(FOnLinkedWallets OnSuccess, FOnLinkedWalletsFailure OnFailure)
{
	const TFunction<void (FSeqLinkedWalletsResponse)> OnApiSuccess = [OnSuccess](const FSeqLinkedWalletsResponse& LinkedWallets)
	{
		OnSuccess.ExecuteIfBound(LinkedWallets);
	};

	const TFunction<void (FSequenceError)> OnApiFailure = [OnFailure](const FSequenceError& Err)
	{
		OnFailure.ExecuteIfBound(Err.Message);
	};
	
	this->Wallet->GetLinkedWallets(OnApiSuccess, OnApiFailure);
}

void USequenceWalletBP::RemoveLinkedWallet(const FString& LinkedWalletAddress, FOnSuccess OnSuccess, FOnLinkedWalletsFailure OnFailure)
{
	const TFunction<void()> OnApiSuccess = [OnSuccess]()
	{
		OnSuccess.ExecuteIfBound();
	};

	const TFunction<void (FSequenceError)> OnApiFailure = [OnFailure](const FSequenceError& Err)
	{
		SEQ_LOG(Error, TEXT("Failed to unlink wallet."));
		OnFailure.ExecuteIfBound(Err.Message);
	};
	
	this->Wallet->RemoveLinkedWallet(LinkedWalletAddress, OnApiSuccess, OnApiFailure);
}

void USequenceWalletBP::CallOnSessionClosed() const
{
	if (this->OnSessionClosed.IsBound())
	{
		this->OnSessionClosed.Broadcast();
	}
}

