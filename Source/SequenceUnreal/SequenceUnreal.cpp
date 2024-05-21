// Copyright Epic Games, Inc. All Rights Reserved.

#include "SequenceUnreal.h"
#include "Modules/ModuleManager.h"
#include "ABI/ABI.h"
#include "Sequence/SequenceAPI.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, SequenceUnreal, "SequenceUnreal" );

void Test()
{
	const FFailureCallback OnFailure = [=](const FSequenceError& Error)
	{
		UE_LOG(LogTemp,Display,TEXT("Error Message: %s"),*Error.Message);
	};
	
    //Create the Transaction object list
	TArray<TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>> Txn;
	
    //Create the transactions you wish to perform

    //ERC20
	FERC20Transaction T20;
	T20.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	T20.tokenAddress = "0x2791Bca1f2de4661ED88A30C99A7a9449Aa84174";
	T20.value = "1000";

    //ERC721

	FERC721Transaction T721;
	T721.safe = true;
	T721.id = "54530968763798660137294927684252503703134533114052628080002308208148824588621";
	T721.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	T721.tokenAddress = "0xa9a6A3626993D487d2Dbda3173cf58cA1a9D9e9f";

    //ERC1155

	FERC1155Transaction T1155;
	T1155.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	T1155.tokenAddress = "0x631998e91476DA5B870D741192fc5Cbc55F5a52E";
	
	FERC1155TxnValue Val;
	Val.amount = "1";
	Val.id = "66635";
	T1155.vals.Add(Val);

    //Raw (Example contract call)

    FString FunctionSignature = "balanceOf(address,uint256)";
	TFixedABIData Account = ABI::Address(FAddress::From("0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2"));
	TFixedABIData Id = ABI::UInt32(0x01);
	TArray<ABIEncodeable*> Arr;
	Arr.Add(&Account);
	Arr.Add(&Id);
	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);

	FRawTransaction T;
	T.data = "0x" + EncodedData.ToHex();
	T.to = "0x64d9f9d527abe2a1c1ce3fada98601c4ac5bfdd2";
	T.value = "0";
    
	Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T20));//ERC20
	Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T721));//ERC721
	Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T1155));//ERC1155
	Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T));//ContractCall

    //Now send the transaction
    const UAuthenticator * Auth = NewObject<UAuthenticator>();   
    const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
    if (WalletOptional.IsSet() && WalletOptional.GetValue())
    {
      USequenceWallet * Api = WalletOptional.GetValue();
       Api->SendTransaction(Txn,[=](FTransactionResponse Transaction)
       {
       	
	   },OnFailure);
    }
}