#include "Integrators/SequenceUtilityBP.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Sequence/Transactions.h"
#include "Util/Log.h"

USequenceUtilityBP::USequenceUtilityBP() { }

USequenceUtilityBP* USequenceUtilityBP::GetSubSystem()
{
	if (GEngine)
	{
		const TIndirectArray<FWorldContext> Contexts = GEngine->GetWorldContexts();
		for (FWorldContext Context : Contexts)
		{
			if (const UWorld* World = Context.World())
			{
				if (const UGameInstance* GI = UGameplayStatics::GetGameInstance(World))
				{
					if (USequenceUtilityBP* Subsystem = GI->GetSubsystem<USequenceUtilityBP>())
					{
						return Subsystem;
					}
				}
			}
		}
	}
	else
	{
		SEQ_LOG(Error,TEXT("Error Accessing GEngine"));
	}
	
	SEQ_LOG(Error,TEXT("Error Accessing USequenceWallet GameInstanceSubSystem"));
	return nullptr;
}

UTransactions* USequenceUtilityBP::ConstructSingleERC20Transaction(const FString& ContractAddress, const FString& RecipientAddress, const FString& Value)
{
	FERC20Transaction Transaction;
	Transaction.tokenAddress = ContractAddress;
	Transaction.to = RecipientAddress;
	Transaction.value = Value;
	
	UTransactions* Transactions = NewObject<UTransactions>();
	Transactions->AddERC20(Transaction);
	
	return Transactions;
}

UTransactions* USequenceUtilityBP::ConstructSingleERC721Transaction(const FString& ContractAddress, const FString& RecipientAddress, const FString& TokenId)
{
	FERC721Transaction Transaction;
	Transaction.safe = true;
	Transaction.tokenAddress = ContractAddress;
	Transaction.to = RecipientAddress;
	Transaction.id = TokenId;
	
	UTransactions* Transactions = NewObject<UTransactions>();
	Transactions->AddERC721(Transaction);
	
	return Transactions;
}

UTransactions* USequenceUtilityBP::ConstructSingleERC1155Transaction(const FString& ContractAddress, const FString& RecipientAddress, const FString& TokenId, const FString& Amount)
{
	FERC1155Transaction Transaction;
	Transaction.tokenAddress = ContractAddress;
	Transaction.to = RecipientAddress;

	FERC1155TxnValue Val;
	Val.id = TokenId;
	Val.amount = Amount;
	Transaction.vals.Add(Val);
	
	UTransactions* Transactions = NewObject<UTransactions>();
	Transactions->AddERC1155(Transaction);
	
	return Transactions;
}
