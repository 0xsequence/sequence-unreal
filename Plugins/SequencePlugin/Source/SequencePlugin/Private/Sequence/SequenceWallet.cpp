// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Sequence/SequenceWallet.h"
#include "Dom/JsonObject.h"
#include "Kismet/GameplayStatics.h"
#include "Types/ContractCall.h"
#include "Misc/Base64.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Indexer/SequenceIndexer.h"
#include "Provider.h"
#include "SequenceRPCManager.h"
#include "Util/Log.h"
#include "Sequence/SequenceSdk.h"
#include "Util/CredentialsStorage.h"

USequenceWallet::USequenceWallet()
{
	this->Indexer = NewObject<USequenceIndexer>();
	this->Provider = UProvider::Make("");
	this->SequenceRPCManager = nullptr;

	SEQ_LOG(Warning,TEXT("Wallet is NOT registered and valid checking on disk credentials"));
	const UCredentialsStorage* CredentialsStorage = NewObject<UCredentialsStorage>();
	FStoredCredentials_BE StoredCredentials = CredentialsStorage->GetStoredCredentials();

	if (StoredCredentials.GetValid())
	{
		SEQ_LOG(Display,TEXT("Successfully loaded on disk credentials"));
		this->Init(StoredCredentials.GetCredentials());
	}
	else
	{
		SEQ_LOG(Warning,TEXT("The Credentials on disk were invalid please login"));
	}
}

void USequenceWallet::Init(const FCredentials_BE& CredentialsIn)
{
	this->Credentials = CredentialsIn;
	this->Indexer = NewObject<USequenceIndexer>();
	this->SequenceRPCManager = USequenceRPCManager::Make(this->Credentials.GetSessionWallet());
	if (!this->Provider)
	{
		this->Provider = UProvider::Make("");
	}
}

void USequenceWallet::Init(const FCredentials_BE& CredentialsIn,const FString& ProviderURL)
{
	this->Credentials = CredentialsIn;
	this->SequenceRPCManager = USequenceRPCManager::Make(this->Credentials.GetSessionWallet());
	if (!this->Provider)
	{
		this->Provider = UProvider::Make(ProviderURL);
	}
	else
	{
		this->Provider->UpdateUrl(ProviderURL);
	}
}

FString USequenceWallet::GetWalletAddress() const
{
	FString Addr;

	if (this->Credentials.IsRegistered())
	{
		Addr = this->Credentials.GetWalletAddress();
	}
	else
	{
		SEQ_LOG(Warning,TEXT("[Please Login first before trying to use credentials]"));
	}
	
	return Addr;
}

void USequenceWallet::GetIdToken(const FString& Nonce, const TSuccessCallback<FSeqIdTokenResponse_Data>&OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->SequenceRPCManager)
	{
		this->SequenceRPCManager->GetIdToken(this->Credentials, Nonce, OnSuccess, OnFailure);
	}
}

void USequenceWallet::ListSessions(const TSuccessCallback<TArray<FSeqListSessions_Session>>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->SequenceRPCManager)
	{
		this->SequenceRPCManager->ListSessions(this->Credentials,OnSuccess,OnFailure);
	}
}

void USequenceWallet::ListAccounts(const TSuccessCallback<FSeqListAccountsResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->SequenceRPCManager)
	{
		this->SequenceRPCManager->ListAccounts(this->Credentials, OnSuccess, OnFailure);
	}
}

void USequenceWallet::GetSessionAuthProof(const FString& Nonce, const TSuccessCallback<FSeqGetSessionAuthProof_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->SequenceRPCManager)
	{
		this->SequenceRPCManager->GetSessionAuthProof(this->Credentials, Nonce, OnSuccess, OnFailure);
	}
}

void USequenceWallet::SignOut() const
{
	const UCredentialsStorage* CredentialsStorage = NewObject<UCredentialsStorage>();
	if (this->Credentials.IsRegistered())
	{
		const TFunction<void()> OnSuccess = [CredentialsStorage]
		{
			CredentialsStorage->ClearStoredCredentials();
		};

		const TFunction<void (FSequenceError)> OnFailure = [CredentialsStorage](FSequenceError Err)
		{
			CredentialsStorage->ClearStoredCredentials();
		};

		if (this->SequenceRPCManager)
		{
			this->SequenceRPCManager->CloseSession(this->Credentials, OnSuccess, OnFailure);
		}
		else
		{
			CredentialsStorage->ClearStoredCredentials();
		}
	}
	else
	{
		CredentialsStorage->ClearStoredCredentials();
	}
}

void USequenceWallet::UpdateProviderURL(const FString& Url) const
{
	if (this->Provider)
	{
		this->Provider->UpdateUrl(Url);
	}
}

void USequenceWallet::SignMessage(const FString& Message, const TSuccessCallback<FSeqSignMessageResponse_Response>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->SequenceRPCManager)
	{
		this->SequenceRPCManager->SignMessage(this->Credentials,Message,OnSuccess,OnFailure);
	}
}


void USequenceWallet::ValidateMessageSignature(const int64& ChainId, const FString& WalletAddress,const FString& Message, const FString& Signature, const TSuccessCallback<FSeqValidateMessageSignatureResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->SequenceRPCManager)
	{
		this->SequenceRPCManager->ValidateMessageSignature(ChainId, WalletAddress, Message, Signature, OnSuccess, OnFailure);
	}
}

void USequenceWallet::SendTransactionWithFeeOption(const TArray<TransactionUnion>& Transactions, const FFeeOption& FeeOption, const TSuccessCallback<FSeqTransactionResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->SequenceRPCManager)
	{
		this->SequenceRPCManager->SendTransactionWithFeeOption(this->Credentials,Transactions,FeeOption,OnSuccess,OnFailure);
	}
}

TArray<FFeeOption> USequenceWallet::BalancesListToFeeOptionList(const TArray<FSeqTokenBalance>& BalanceList)
{
	TArray<FFeeOption> ParsedBalances;

	for (auto Balance : BalanceList)
	{
		ParsedBalances.Add(FFeeOption(Balance));
	}
	return ParsedBalances;
}

TArray<FFeeOption> USequenceWallet::MarkValidFeeOptions(TArray<FFeeOption> FeeOptions, TArray<FFeeOption> BalanceOptions)
{
	for (int i = 0; i < FeeOptions.Num(); i++)
	{
		for (int j = 0; j < BalanceOptions.Num(); j++)
		{
			FeeOptions[i].bCanAfford |= BalanceOptions[j].CanAfford(FeeOptions[i]);
		}
	}
	return FeeOptions;
}

TArray<FFeeOption> USequenceWallet::FindValidFeeOptions(const TArray<FFeeOption>& FeeOptions, const TArray<FFeeOption>& BalanceOptions)
{
	TArray<FFeeOption> ValidFeeOptions;

	for (auto BalanceOption : BalanceOptions)
	{
		for (auto FeeOption : FeeOptions)
		{
			if (BalanceOption.CanAfford(FeeOption))
			{
				FeeOption.bCanAfford = true;
				ValidFeeOptions.Add(FeeOption);
			}
		}
	}
	
	return ValidFeeOptions;
}

void USequenceWallet::GetFeeOptions(const TArray<TransactionUnion>& Transactions, const TSuccessCallback<TArray<FFeeOption>>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const TSuccessCallback<TArray<FFeeOption>> OnResponse = [this, OnSuccess, OnFailure](const TArray<FFeeOption>& Fees)
	{
		FSeqGetTokenBalancesArgs Args;
		Args.accountAddress = this->GetWalletAddress();
		Args.includeMetaData = true;

		const TSuccessCallback<FSeqGetTokenBalancesReturn> BalanceSuccess = [this, Fees, OnSuccess, OnFailure] (const FSeqGetTokenBalancesReturn& BalanceResponse)
		{
			TArray<FSeqTokenBalance> PreProcBalances = BalanceResponse.balances;
			
			const TSuccessCallback<FSeqEtherBalance> EtherSuccess = [this, PreProcBalances, Fees, OnSuccess] (const FSeqEtherBalance& EtherBalance)
			{
				TArray<FFeeOption> Balances = this->BalancesListToFeeOptionList(PreProcBalances);
				Balances.Add(FFeeOption(EtherBalance));
				OnSuccess(this->FindValidFeeOptions(Fees,Balances));
			};

			const FFailureCallback EtherFailure = [this,OnFailure] (const FSequenceError& Err)
			{
				OnFailure(FSequenceError(RequestFail, "Failed to Get EtherBalance from Indexer"));
			};

			this->Indexer->GetNativeTokenBalance(SequenceSdk::GetChainId(), this->GetWalletAddress(),EtherSuccess,EtherFailure);
		};

		const FFailureCallback BalanceFailure = [this, OnFailure](const FSequenceError& Err)
		{
			OnFailure(FSequenceError(RequestFail, "Failed to Get Balances from Indexer"));
		};
						
		this->Indexer->GetTokenBalances(SequenceSdk::GetChainId(), Args,BalanceSuccess,BalanceFailure);
	};

	if (this->SequenceRPCManager)
	{
		this->SequenceRPCManager->GetFeeOptions(this->Credentials,Transactions,OnResponse,OnFailure);
	}
}

void USequenceWallet::GetUnfilteredFeeOptions(const TArray<TransactionUnion>& Transactions, const TSuccessCallback<TArray<FFeeOption>>& OnSuccess, const FFailureCallback& OnFailure) const
{	
	const TSuccessCallback<TArray<FFeeOption>> OnResponse = [this, OnSuccess, OnFailure](const TArray<FFeeOption>& Fees)
	{
		const TSuccessCallback<FSeqGetTokenBalancesReturn> BalanceSuccess = [this, Fees, OnSuccess, OnFailure] (const FSeqGetTokenBalancesReturn& BalanceResponse)
		{
			TArray<FSeqTokenBalance> PreProcBalances = BalanceResponse.balances;
			const TSuccessCallback<FSeqEtherBalance> EtherSuccess = [this, PreProcBalances, Fees, OnSuccess] (const FSeqEtherBalance& EtherBalance)
			{
				TArray<FFeeOption> Balances = this->BalancesListToFeeOptionList(PreProcBalances);
				Balances.Add(FFeeOption(EtherBalance));
				OnSuccess(this->MarkValidFeeOptions(Fees,Balances));
			};

			const FFailureCallback EtherFailure = [this,OnFailure] (const FSequenceError& Err)
			{
				OnFailure(FSequenceError(RequestFail, "Failed to Get EtherBalance from Indexer"));
			};

			this->Indexer->GetNativeTokenBalance(SequenceSdk::GetChainId(), this->GetWalletAddress(),EtherSuccess,EtherFailure);
		};

		const FFailureCallback BalanceFailure = [this, OnFailure](const FSequenceError& Err)
		{
			OnFailure(FSequenceError(RequestFail, "Failed to Get Balances from Indexer"));
		};

		FSeqGetTokenBalancesArgs Args;
		Args.accountAddress = this->GetWalletAddress();
		Args.includeMetaData = true;
		
		this->Indexer->GetTokenBalances(SequenceSdk::GetChainId(),Args,BalanceSuccess,BalanceFailure);
	};

	if (this->SequenceRPCManager)
	{
		this->SequenceRPCManager->GetFeeOptions(this->Credentials,Transactions,OnResponse,OnFailure);
	}
}

void USequenceWallet::SendNativeToken(const FString& RecipientAddress, const FString& Amount, const TSuccessCallback<FSeqTransactionResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	FRawTransaction T;
	T.data = "0x0";
	T.to = RecipientAddress;
	T.value = Amount;
	
	TArray<TransactionUnion> Transactions;
	Transactions.Push(TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction, FDelayedTransaction>(T));
	
	this->SendTransaction(Transactions, OnSuccess, OnFailure);
}

void USequenceWallet::SendTransaction(const TArray<TransactionUnion>& Transactions, const TSuccessCallback<FSeqTransactionResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->SequenceRPCManager)
	{
		this->SequenceRPCManager->SendTransaction(this->Credentials, Transactions, OnSuccess, OnFailure);
	}
}

void USequenceWallet::BlockByNumber(uint64 Number, const TSuccessCallback<TSharedPtr<FJsonObject>>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->BlockByNumber(Number, OnSuccess, OnFailure);
	}
}

void USequenceWallet::BlockByNumber(EBlockTag Tag, const TSuccessCallback<TSharedPtr<FJsonObject>>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->BlockByNumber(Tag,OnSuccess,OnFailure);
	}
}

void USequenceWallet::BlockByHash(const FHash256& Hash, const TSuccessCallback<TSharedPtr<FJsonObject>>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->BlockByHash(Hash, OnSuccess, OnFailure);
	}
}

void USequenceWallet::BlockNumber(const TSuccessCallback<uint64>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->BlockNumber(OnSuccess, OnFailure);
	}
}

void USequenceWallet::HeaderByNumber(uint64 Id, const TSuccessCallback<FHeader>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->HeaderByNumber(Id, OnSuccess, OnFailure);
	}
}

void USequenceWallet::HeaderByNumber(EBlockTag Tag, const TSuccessCallback<FHeader>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->HeaderByNumber(Tag, OnSuccess, OnFailure);
	}
}

void USequenceWallet::HeaderByHash(const FHash256& Hash, const TSuccessCallback<FHeader>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->HeaderByHash(Hash,OnSuccess,OnFailure);
	}
}

void USequenceWallet::TransactionByHash(const FHash256& Hash, const TSuccessCallback<TSharedPtr<FJsonObject>>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->TransactionByHash(Hash,OnSuccess,OnFailure);
	}
}

void USequenceWallet::TransactionCount(const FAddress& Addr, uint64 Number, const TSuccessCallback<uint64>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->TransactionCount(Addr,Number,OnSuccess,OnFailure);	
	}
}

void USequenceWallet::TransactionCount(const FAddress& Addr, EBlockTag Tag, const TSuccessCallback<uint64>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->TransactionCount(Addr,Tag,OnSuccess,OnFailure);
	}
}

void USequenceWallet::TransactionReceipt(const FHash256& Hash, const TSuccessCallback<FTransactionReceipt>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->TransactionReceipt(Hash,OnSuccess,OnFailure);
	}
}

void USequenceWallet::GetGasPrice(const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->GetGasPrice(OnSuccess, OnFailure);
	}
}

void USequenceWallet::EstimateContractCallGas(const FContractCall& ContractCall, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->EstimateContractCallGas(ContractCall,OnSuccess,OnFailure);
	}
}

void USequenceWallet::NonceAt(uint64 Number, const TSuccessCallback<FBlockNonce>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->NonceAt(Number,OnSuccess,OnFailure);
	}
}

void USequenceWallet::NonceAt(EBlockTag Tag, const TSuccessCallback<FBlockNonce>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->NonceAt(Tag,OnSuccess,OnFailure);
	}
}

void USequenceWallet::SendRawTransaction(const FString& Data, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->SendRawTransaction(Data,OnSuccess,OnFailure);
	}
}

void USequenceWallet::ChainId(const TSuccessCallback<uint64>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->ChainId(OnSuccess,OnFailure);
	}
}

void USequenceWallet::Call(const FContractCall& ContractCall, uint64 Number, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->Call(ContractCall,Number,OnSuccess,OnFailure);
	}
}

void USequenceWallet::Call(const FContractCall& ContractCall, EBlockTag Number, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->Call(ContractCall,Number,OnSuccess,OnFailure);
	}
}

void USequenceWallet::NonViewCall(const FEthTransaction& Transaction, const FPrivateKey& PrivateKey, int ChainID, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->NonViewCall(Transaction, PrivateKey, ChainID, OnSuccess, OnFailure);
	}
}