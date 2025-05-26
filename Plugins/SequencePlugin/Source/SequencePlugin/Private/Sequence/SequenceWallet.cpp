#include "Sequence/SequenceWallet.h"
#include "Dom/JsonObject.h"
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
	this->Provider = NewObject<UProvider>();
	this->SequenceRPCManager = USequenceRPCManager::Make(false);
	this->CredentialsStorage = NewObject<UCredentialsStorage>();
}

bool USequenceWallet::IsValidSession()
{
	switch (this->SessionState)
	{
		case ESessionState::Valid:
			return true;
		case ESessionState::Invalid:
			return false;
		case ESessionState::Unchecked:
			{
				const TSuccessCallback<TArray<FSeqListSessions_Session>> OnResponse = [this](TArray<FSeqListSessions_Session> Response)
				{
					this->SessionState = ESessionState::Valid;
				};

				const FFailureCallback GenericFailure = [this](const FSequenceError& Error)
				{
					UE_LOG(LogTemp, Error, TEXT("Error recovering session: %s"), *Error.Message);
					this->SessionState = ESessionState::Invalid;
					this->GetCredentials().UnRegisterCredentials();
					this->CredentialsStorage->ClearStoredCredentials();
				this->SessionState = ESessionState::Checking;
				};
				this->ListSessions(OnResponse, GenericFailure);
				return true;
			}
		default:
			return true;
	}
}

FString USequenceWallet::GetWalletAddress() const
{
	FString Addr;

	if (this->GetCredentials().IsRegistered())
	{
		Addr = this->GetCredentials().GetWalletAddress();
	}
	else
	{
		SEQ_LOG(Warning, TEXT("Please Login first before trying to use credentials."));
	}
	
	return Addr;
}

FString USequenceWallet::GetSessionId() const
{
	return this->GetCredentials().GetSessionWallet()->GetSessionId();
}

void USequenceWallet::GetIdToken(const FString& Nonce, const TSuccessCallback<FSeqIdTokenResponse_Data>&OnSuccess, const FFailureCallback& OnFailure) const
{
	this->GetRpcManager()->GetIdToken(this->GetCredentials(), Nonce, OnSuccess, OnFailure);
}

void USequenceWallet::ListSessions(const TSuccessCallback<TArray<FSeqListSessions_Session>>& OnSuccess, const FFailureCallback& OnFailure) const
{
	this->GetRpcManager()->ListSessions(this->GetCredentials(),OnSuccess,OnFailure);
}

void USequenceWallet::ListAccounts(const TSuccessCallback<FSeqListAccountsResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	this->GetRpcManager()->ListAccounts(this->GetCredentials(), OnSuccess, OnFailure);
}

void USequenceWallet::GetSessionAuthProof(const FString& Nonce, const TSuccessCallback<FSeqGetSessionAuthProof_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	this->GetRpcManager()->GetSessionAuthProof(this->GetCredentials(), Nonce, OnSuccess, OnFailure);
}

void USequenceWallet::SignOut() const
{
	const TFunction<void()> OnSuccess = [this] { };
	const TFunction<void (FSequenceError)> OnFailure = [this](FSequenceError Err) { };

	if (const FCredentials_BE Credentials = this->GetCredentials(); Credentials.IsRegistered())
		this->GetRpcManager()->CloseSession(Credentials, OnSuccess, OnFailure);
		
	this->CredentialsStorage->ClearStoredCredentials();
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
	this->GetRpcManager()->SignMessage(this->GetCredentials(),Message,OnSuccess,OnFailure);
}

void USequenceWallet::ValidateMessageSignature(const int64& ChainId, const FString& WalletAddress,const FString& Message, const FString& Signature, const TSuccessCallback<FSeqValidateMessageSignatureResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	this->GetRpcManager()->ValidateMessageSignature(ChainId, WalletAddress, Message, Signature, OnSuccess, OnFailure);
}

void USequenceWallet::SendTransactionWithFeeOption(const TArray<TransactionUnion>& Transactions, const FFeeOption& FeeOption, const TSuccessCallback<FSeqTransactionResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	this->GetRpcManager()->SendTransactionWithFeeOption(this->GetCredentials(),Transactions,FeeOption,OnSuccess,OnFailure);
}

USequenceRPCManager* USequenceWallet::GetRpcManager() const
{
	this->SequenceRPCManager->UpdateWithStoredSessionWallet();
	return this->SequenceRPCManager;
}

FCredentials_BE USequenceWallet::GetCredentials() const
{
	return this->CredentialsStorage->GetStoredCredentials().GetCredentials();
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

	this->GetRpcManager()->GetFeeOptions(this->GetCredentials(),Transactions,OnResponse,OnFailure);
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

	this->GetRpcManager()->GetFeeOptions(this->GetCredentials(),Transactions,OnResponse,OnFailure);
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
	this->GetRpcManager()->SendTransaction(this->GetCredentials(), Transactions, OnSuccess, OnFailure);
}

void USequenceWallet::GetLinkedWallets(const TSuccessCallback<FSeqLinkedWalletsResponse>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const FString& WalletAddress = this->GetWalletAddress();
	const FString& MessageToSign = "parent wallet with address " + WalletAddress;

	const TSuccessCallback<FSeqSignMessageResponse_Response> OnSignatureSuccess = [this, WalletAddress, MessageToSign, OnSuccess, OnFailure](FSeqSignMessageResponse_Response SignatureResponse)
	{
		const FString& ChainId = FString::FromInt(SequenceSdk::GetChainId());
		FSeqLinkedWalletRequest Request;
		Request.SignatureChainId = ChainId;
		Request.ParentWalletAddress = WalletAddress;
		Request.ParentWalletMessage = MessageToSign;
		Request.ParentWalletSignature = SignatureResponse.Data.Signature;
			
		this->GetRpcManager()->GetLinkedWallets(Request, OnSuccess, OnFailure);
	};

	const TFunction<void (FString, FSequenceError)> OnSignatureFailure = [OnFailure](FString Data, FSequenceError Err)
	{
		OnFailure(FSequenceError(RequestFail, "Error Parsing Response: " + Err.Message));
	};
	
	this->SignMessage(MessageToSign, OnSignatureSuccess, OnFailure);
}

void USequenceWallet::RemoveLinkedWallet(const FString& LinkedWalletAddress, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const FString& WalletAddress = this->GetWalletAddress();
	const FString& MessageToSign = "parent wallet with address " + WalletAddress + LinkedWalletAddress;

	const TSuccessCallback<FSeqSignMessageResponse_Response> OnSignatureSuccess = [this, LinkedWalletAddress, WalletAddress, MessageToSign, OnSuccess, OnFailure](FSeqSignMessageResponse_Response SignatureResponse)
	{
		const FString& ChainId = FString::FromInt(SequenceSdk::GetChainId());
		FSeqLinkedWalletRequest Request;
		Request.SignatureChainId = ChainId;
		Request.ParentWalletAddress = WalletAddress;
		Request.ParentWalletMessage = MessageToSign;
		Request.ParentWalletSignature = SignatureResponse.Data.Signature;
		Request.LinkedWalletAddress = LinkedWalletAddress;
			
		this->GetRpcManager()->RemoveLinkedWallet(Request, OnSuccess, OnFailure);
	};

	const TFunction<void (FString, FSequenceError)> OnSignatureFailure = [OnFailure](FString Data, FSequenceError Err)
	{
		OnFailure(FSequenceError(RequestFail, "Error Parsing Response: " + Err.Message));
	};
	
	this->SignMessage(MessageToSign, OnSignatureSuccess, OnFailure);
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

void USequenceWallet::Call(const FContractCall& ContractCall, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->Call(ContractCall,EBlockTag::ELatest,OnSuccess,OnFailure);
	}
}

void USequenceWallet::Call(const FContractCall& ContractCall, EBlockTag Number, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const
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