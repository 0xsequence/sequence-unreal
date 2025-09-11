#include "Sequence/SequenceEmbeddedWallet.h"
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

USequenceEmbeddedWallet::USequenceEmbeddedWallet()
{
	this->Indexer = NewObject<USequenceIndexer>();
	this->Provider = NewObject<UProvider>();
	this->SequenceRPCManager = USequenceRPCManager::Make(false);
	this->CredentialsStorage = NewObject<UCredentialsStorage>();
}

bool USequenceEmbeddedWallet::IsValidSession()
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

FString USequenceEmbeddedWallet::GetWalletAddress() const
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

FString USequenceEmbeddedWallet::GetSessionId() const
{
	return this->GetCredentials().GetSessionWallet()->GetSessionId();
}

void USequenceEmbeddedWallet::GetIdToken(const FString& Nonce, const TSuccessCallback<FSeqIdTokenResponse_Data>&OnSuccess, const FFailureCallback& OnFailure) const
{
	this->GetRpcManager()->GetIdToken(this->GetCredentials(), Nonce, OnSuccess, OnFailure);
}

void USequenceEmbeddedWallet::ListSessions(const TSuccessCallback<TArray<FSeqListSessions_Session>>& OnSuccess, const FFailureCallback& OnFailure) const
{
	this->GetRpcManager()->ListSessions(this->GetCredentials(),OnSuccess,OnFailure);
}

void USequenceEmbeddedWallet::ListAccounts(const TSuccessCallback<FSeqListAccountsResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	this->GetRpcManager()->ListAccounts(this->GetCredentials(), OnSuccess, OnFailure);
}

void USequenceEmbeddedWallet::GetSessionAuthProof(const FString& Nonce, const TSuccessCallback<FSeqGetSessionAuthProof_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	this->GetRpcManager()->GetSessionAuthProof(this->GetCredentials(), Nonce, OnSuccess, OnFailure);
}

void USequenceEmbeddedWallet::SignOut() const
{
	const TFunction<void()> OnSuccess = [this] { };
	const TFunction<void (FSequenceError)> OnFailure = [this](FSequenceError Err) { };

	if (const FCredentials_BE Credentials = this->GetCredentials(); Credentials.IsRegistered())
		this->GetRpcManager()->CloseSession(Credentials, OnSuccess, OnFailure);
		
	this->CredentialsStorage->ClearStoredCredentials();
}

void USequenceEmbeddedWallet::UpdateProviderURL(const FString& Url) const
{
	if (this->Provider)
	{
		this->Provider->UpdateUrl(Url);
	}
}

void USequenceEmbeddedWallet::SignMessage(const FString& Message, const TSuccessCallback<FSeqSignMessageResponse_Response>& OnSuccess, const FFailureCallback& OnFailure) const
{
	this->GetRpcManager()->SignMessage(this->GetCredentials(),Message,OnSuccess,OnFailure);
}

void USequenceEmbeddedWallet::ValidateMessageSignature(const int64& ChainId, const FString& WalletAddress,const FString& Message, const FString& Signature, const TSuccessCallback<FSeqValidateMessageSignatureResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	this->GetRpcManager()->ValidateMessageSignature(ChainId, WalletAddress, Message, Signature, OnSuccess, OnFailure);
}

void USequenceEmbeddedWallet::SendTransactionWithFeeOption(const TArray<TransactionUnion>& Transactions, const FFeeOption& FeeOption, const TSuccessCallback<FSeqTransactionResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	this->GetRpcManager()->SendTransactionWithFeeOption(this->GetCredentials(),Transactions,FeeOption,OnSuccess,OnFailure);
}

USequenceRPCManager* USequenceEmbeddedWallet::GetRpcManager() const
{
	this->SequenceRPCManager->UpdateWithStoredSessionWallet();
	return this->SequenceRPCManager;
}

FCredentials_BE USequenceEmbeddedWallet::GetCredentials() const
{
	return this->CredentialsStorage->GetStoredCredentials().GetCredentials();
}

TArray<FFeeOption> USequenceEmbeddedWallet::BalancesListToFeeOptionList(const TArray<FSeqTokenBalance>& BalanceList)
{
	TArray<FFeeOption> ParsedBalances;

	for (auto Balance : BalanceList)
	{
		ParsedBalances.Add(FFeeOption(Balance));
	}
	return ParsedBalances;
}

TArray<FFeeOption> USequenceEmbeddedWallet::MarkValidFeeOptions(TArray<FFeeOption> FeeOptions, TArray<FFeeOption> BalanceOptions)
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

TArray<FFeeOption> USequenceEmbeddedWallet::FindValidFeeOptions(const TArray<FFeeOption>& FeeOptions, const TArray<FFeeOption>& BalanceOptions)
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

void USequenceEmbeddedWallet::GetFeeOptions(const TArray<TransactionUnion>& Transactions, const TSuccessCallback<TArray<FFeeOption>>& OnSuccess, const FFailureCallback& OnFailure) const
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

void USequenceEmbeddedWallet::GetUnfilteredFeeOptions(const TArray<TransactionUnion>& Transactions, const TSuccessCallback<TArray<FFeeOption>>& OnSuccess, const FFailureCallback& OnFailure) const
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

void USequenceEmbeddedWallet::SendNativeToken(const FString& RecipientAddress, const FString& Amount, const TSuccessCallback<FSeqTransactionResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	FRawTransaction T;
	T.data = "0x0";
	T.to = RecipientAddress;
	T.value = Amount;
	
	TArray<TransactionUnion> Transactions;
	Transactions.Push(TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction, FDelayedTransaction>(T));
	
	this->SendTransaction(Transactions, OnSuccess, OnFailure);
}

void USequenceEmbeddedWallet::SendTransaction(const TArray<TransactionUnion>& Transactions, const TSuccessCallback<FSeqTransactionResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	this->GetRpcManager()->SendTransaction(this->GetCredentials(), Transactions, OnSuccess, OnFailure);
}

void USequenceEmbeddedWallet::GetLinkedWallets(const TSuccessCallback<FSeqLinkedWalletsResponse>& OnSuccess, const FFailureCallback& OnFailure) const
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

void USequenceEmbeddedWallet::RemoveLinkedWallet(const FString& LinkedWalletAddress, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure) const
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

void USequenceEmbeddedWallet::BlockByNumber(uint64 Number, const TSuccessCallback<TSharedPtr<FJsonObject>>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->BlockByNumber(Number, OnSuccess, OnFailure);
	}
}

void USequenceEmbeddedWallet::BlockByNumber(EBlockTag Tag, const TSuccessCallback<TSharedPtr<FJsonObject>>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->BlockByNumber(Tag,OnSuccess,OnFailure);
	}
}

void USequenceEmbeddedWallet::BlockByHash(const FHash256& Hash, const TSuccessCallback<TSharedPtr<FJsonObject>>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->BlockByHash(Hash, OnSuccess, OnFailure);
	}
}

void USequenceEmbeddedWallet::BlockNumber(const TSuccessCallback<uint64>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->BlockNumber(OnSuccess, OnFailure);
	}
}

void USequenceEmbeddedWallet::HeaderByNumber(uint64 Id, const TSuccessCallback<FHeader>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->HeaderByNumber(Id, OnSuccess, OnFailure);
	}
}

void USequenceEmbeddedWallet::HeaderByNumber(EBlockTag Tag, const TSuccessCallback<FHeader>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->HeaderByNumber(Tag, OnSuccess, OnFailure);
	}
}

void USequenceEmbeddedWallet::HeaderByHash(const FHash256& Hash, const TSuccessCallback<FHeader>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->HeaderByHash(Hash,OnSuccess,OnFailure);
	}
}

void USequenceEmbeddedWallet::TransactionByHash(const FHash256& Hash, const TSuccessCallback<TSharedPtr<FJsonObject>>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->TransactionByHash(Hash,OnSuccess,OnFailure);
	}
}

void USequenceEmbeddedWallet::TransactionCount(const FAddress& Addr, uint64 Number, const TSuccessCallback<uint64>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->TransactionCount(Addr,Number,OnSuccess,OnFailure);	
	}
}

void USequenceEmbeddedWallet::TransactionCount(const FAddress& Addr, EBlockTag Tag, const TSuccessCallback<uint64>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->TransactionCount(Addr,Tag,OnSuccess,OnFailure);
	}
}

void USequenceEmbeddedWallet::TransactionReceipt(const FHash256& Hash, const TSuccessCallback<FTransactionReceipt>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->TransactionReceipt(Hash,OnSuccess,OnFailure);
	}
}

void USequenceEmbeddedWallet::GetGasPrice(const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->GetGasPrice(OnSuccess, OnFailure);
	}
}

void USequenceEmbeddedWallet::EstimateContractCallGas(const FContractCall& ContractCall, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->EstimateContractCallGas(ContractCall,OnSuccess,OnFailure);
	}
}

void USequenceEmbeddedWallet::NonceAt(uint64 Number, const TSuccessCallback<FBlockNonce>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->NonceAt(Number,OnSuccess,OnFailure);
	}
}

void USequenceEmbeddedWallet::NonceAt(EBlockTag Tag, const TSuccessCallback<FBlockNonce>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->NonceAt(Tag,OnSuccess,OnFailure);
	}
}

void USequenceEmbeddedWallet::SendRawTransaction(const FString& Data, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->SendRawTransaction(Data,OnSuccess,OnFailure);
	}
}

void USequenceEmbeddedWallet::ChainId(const TSuccessCallback<uint64>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->ChainId(OnSuccess,OnFailure);
	}
}

void USequenceEmbeddedWallet::Call(const FContractCall& ContractCall, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->Call(ContractCall,EBlockTag::ELatest,OnSuccess,OnFailure);
	}
}

void USequenceEmbeddedWallet::Call(const FContractCall& ContractCall, const uint64 Number, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->Call(ContractCall,Number,OnSuccess,OnFailure);
	}
}

void USequenceEmbeddedWallet::Call(const FContractCall& ContractCall, const EBlockTag Number, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->Call(ContractCall,Number,OnSuccess,OnFailure);
	}
}

void USequenceEmbeddedWallet::NonViewCall(const FEthTransaction& Transaction, const FPrivateKey& PrivateKey, int ChainID, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->NonViewCall(Transaction, PrivateKey, ChainID, OnSuccess, OnFailure);
	}
}