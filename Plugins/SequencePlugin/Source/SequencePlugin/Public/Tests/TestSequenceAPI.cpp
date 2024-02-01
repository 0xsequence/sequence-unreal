#include "TestSequenceAPI.h"
#include "Sequence/SequenceAPI.h"
#include "Authenticator.h"

void SequenceAPITest::RegisterSession(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	USequenceWallet * Api = USequenceWallet::Make(Auth->GetStoredCredentials().GetCredentials());

	const TFunction<void(FString)> OnResponse = [OnSuccess](FString Response)
	{
		OnSuccess("RegisterSession Test Passed");	
	};
	
	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API RegisterSession Test]========================"));

	Api->RegisterSession(OnSuccess,GenericFailure);
}

void SequenceAPITest::SignMessage(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	USequenceWallet * Api = USequenceWallet::Make(Auth->GetStoredCredentials().GetCredentials());

	const TSuccessCallback<FSignedMessage> OnResponse = [OnSuccess] (FSignedMessage response)
	{
		OnSuccess("Sign Message Test Passed");
	};
	
	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Sign Message Test Failed", Error);
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API SignMessage Test]========================"));

	Api->SignMessage("hi",OnResponse,GenericFailure);
}

void SequenceAPITest::ListSessions(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	USequenceWallet * Api = USequenceWallet::Make(Auth->GetStoredCredentials().GetCredentials());

	const TSuccessCallback<TArray<FSession>> OnResponse = [OnSuccess](TArray<FSession> response)
	{
		OnSuccess("List Sessions Test Passed");
	};
	
	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API ListSessions Test]========================"));

	Api->ListSessions(OnResponse,GenericFailure);
}

void SequenceAPITest::SendRaw(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	USequenceWallet * Api = USequenceWallet::Make(Auth->GetStoredCredentials().GetCredentials());

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API Send[Raw]Transaction Test]========================"));
	
	TArray<TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>> Txn;
	FRawTransaction T;
	T.data = "data?";
	T.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	T.value = "0";
	
	Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T));
	Api->SendTransaction(Txn,OnSuccess,GenericFailure);
}
void SequenceAPITest::SendERC20(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	USequenceWallet * Api = USequenceWallet::Make(Auth->GetStoredCredentials().GetCredentials());

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API Send[ERC20]Transaction Test]========================"));
	
	TArray<TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>> Txn;
	
	FERC20Transaction T20;
	T20.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	T20.token = "0x2791Bca1f2de4661ED88A30C99A7a9449Aa84174";
	T20.value = "100000";
	
	Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T20));
	Api->SendTransaction(Txn,OnSuccess,GenericFailure);
}
void SequenceAPITest::SendERC721(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	USequenceWallet * Api = USequenceWallet::Make(Auth->GetStoredCredentials().GetCredentials());

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API Send[ERC721]Transaction Test]========================"));
	
	TArray<TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>> Txn;

	FERC721Transaction T721;
	T721.safe = true;
	T721.id = "54530968763798660137294927684252503703134533114052628080002308208148824588621";
	T721.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	T721.token = "0xa9a6A3626993D487d2Dbda3173cf58cA1a9D9e9f";
	
	Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T721));
	Api->SendTransaction(Txn,OnSuccess,GenericFailure);
}
void SequenceAPITest::SendERC1155(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	USequenceWallet * Api = USequenceWallet::Make(Auth->GetStoredCredentials().GetCredentials());

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API Send[ERC1155]Transaction Test]========================"));
	
	TArray<TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>> Txn;
	FERC1155Transaction T1155;
	T1155.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	T1155.token = "0x631998e91476DA5B870D741192fc5Cbc55F5a52E";
	
	FERC1155TxnValue val;
	val.amount = "1";
	val.id = "66635";
	T1155.vals.Add(val);

	Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T1155));
	Api->SendTransaction(Txn,OnSuccess,GenericFailure);
}

void SequenceAPITest::SendTransaction(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	USequenceWallet * Api = USequenceWallet::Make(Auth->GetStoredCredentials().GetCredentials());

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API SendTransaction Test]========================"));
	
	TArray<TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>> Txn;
	FRawTransaction T;
	T.data = "data?";
	T.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	T.value = "0";
	
	FERC20Transaction T20;
	T20.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	T20.token = "0x2791Bca1f2de4661ED88A30C99A7a9449Aa84174";
	T20.value = "100000";

	FERC721Transaction T721;
	T721.safe = true;
	T721.id = "54530968763798660137294927684252503703134533114052628080002308208148824588621";
	T721.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	T721.token = "0xa9a6A3626993D487d2Dbda3173cf58cA1a9D9e9f";

	FERC1155Transaction T1155;
	T1155.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	T1155.token = "0x631998e91476DA5B870D741192fc5Cbc55F5a52E";
	
	FERC1155TxnValue val;
	val.amount = "1";
	val.id = "66635";
	T1155.vals.Add(val);
	
	//Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T));
	//Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T20));
	//Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T721));
	//Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T1155));
	
	//Api->SendTransaction(Txn,OnSuccess,GenericFailure);
}

void SequenceAPITest::CloseSession(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	USequenceWallet * Api = USequenceWallet::Make(Auth->GetStoredCredentials().GetCredentials());

	const TFunction<void(void)> OnResponse = [OnSuccess]()
	{
		OnSuccess("CloseSession Test Passed");
	};
	
	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API CloseSession Test]========================"));

	Api->CloseSession(OnResponse,GenericFailure);
}