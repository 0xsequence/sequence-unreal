// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "TestSequenceAPI.h"
#include "Sequence/SequenceAPI.h"
#include "Authenticator.h"
#include "ABI/ABI.h"
#include "Native/NativeOAuth.h"

/*
 * testing stack based implementation for memory faults
 */
void SequenceAPITest::BasicProviderTests()
{
	const TFunction<void(FUnsizedData)> TestResponse = [](FUnsizedData Response)
	{
		UE_LOG(LogTemp,Display,TEXT("Basic provider response"));
	};

	const TFunction<void(FSequenceError)> TestError = [](FSequenceError Error)
	{
		UE_LOG(LogTemp,Display,TEXT("Error"));
	};

	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials(),"https://cognito-idp.us-east-2.amazonaws.com");
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		const FContractCall TestCall;
		Api->Call(TestCall,6,TestResponse,TestError);
	}
}

void SequenceAPITest::RegisterSession(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
#if PLATFORM_ANDROID
	NativeOAuth::AndroidLog("RegisterSession");
#endif
	const TFunction<void(FCredentials_BE)> OnResponse = [OnSuccess](FCredentials_BE Response)
	{
		OnSuccess("RegisterSession Test Passed");
#if PLATFORM_ANDROID
		NativeOAuth::AndroidLog("RegisterSessionDone");
#endif
	};
	
	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
#if PLATFORM_ANDROID
		NativeOAuth::AndroidLog("RegisterSessionFail");
#endif
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API RegisterSession Test]========================"));

	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		Api->RegisterSession(OnResponse,GenericFailure);
	}
}

void SequenceAPITest::SignMessage(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
#if PLATFORM_ANDROID
	NativeOAuth::AndroidLog("SignMessage");
#endif
	
	const TSuccessCallback<FSignedMessage> OnResponse = [OnSuccess] (FSignedMessage Response)
	{
		OnSuccess("Sign Message Test Passed");
#if PLATFORM_ANDROID
		NativeOAuth::AndroidLog("SignMessageDone");
#endif
	};
	
	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Sign Message Test Failed", Error);
#if PLATFORM_ANDROID
		NativeOAuth::AndroidLog("SignMessageFail");
#endif
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API SignMessage Test]========================"));
	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		Api->SignMessage("hi",OnResponse,GenericFailure);
	}
}

void SequenceAPITest::ListSessions(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
#if PLATFORM_ANDROID
	NativeOAuth::AndroidLog("ListSessions");
#endif
	const TSuccessCallback<TArray<FSession>> OnResponse = [OnSuccess](TArray<FSession> Response)
	{
		OnSuccess("List Sessions Test Passed");
#if PLATFORM_ANDROID
		NativeOAuth::AndroidLog("ListSessionsDone");
#endif
	};
	
	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
#if PLATFORM_ANDROID
		NativeOAuth::AndroidLog("ListSessionsFail");
#endif
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API ListSessions Test]========================"));
	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		Api->ListSessions(OnResponse,GenericFailure);
	}
}

void SequenceAPITest::SendRaw(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
#if PLATFORM_ANDROID
	NativeOAuth::AndroidLog("SendTransaction");
#endif
	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
#if PLATFORM_ANDROID
		NativeOAuth::AndroidLog("SendTransactionFail");
#endif
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API Send[Raw]Transaction Test]========================"));
	
	TArray<TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>> Txn;
	FRawTransaction T;
	T.data = "data?";
	T.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	T.value = "0";
	
	Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T));

	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		Api->SendTransaction(Txn,[=](FTransactionResponse Transaction)
		{
			FString OutputString;
			TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
			FJsonSerializer::Serialize(Transaction.Json.ToSharedRef(), Writer);
			OnSuccess(OutputString);
	#if PLATFORM_ANDROID
	NativeOAuth::AndroidLog("SendTransactionDone");
	#endif
		},GenericFailure);
	}
}

void SequenceAPITest::CallContract(TFunction<void(FString)> OnSuccess,
	TFunction<void(FString, FSequenceError)> OnFailure)
{
#if PLATFORM_ANDROID
	NativeOAuth::AndroidLog("CallContract");
#endif

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
#if PLATFORM_ANDROID
		NativeOAuth::AndroidLog("CallContractFailed");
#endif
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API Call Contract Test]========================"));
	
	// CALL DATA
	FString FunctionSignature = "balanceOf(address,uint256)";
	TFixedABIData Account = ABI::Address(FAddress::From("0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2"));
	TFixedABIData Id = ABI::UInt32(0x01);
	TArray<ABIEncodeable*> Arr;
	Arr.Add(&Account);
	Arr.Add(&Id);
	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);
	
	
	TArray<TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>> Txn;
	FRawTransaction T;
	
	T.data = "0x" + EncodedData.ToHex();
	T.to = "0x64d9f9d527abe2a1c1ce3fada98601c4ac5bfdd2";
	T.value = "0";
	
	Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T));

	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		Api->SendTransaction(Txn,[=](FTransactionResponse Transaction)
		{
			FString OutputString;
			TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
			FJsonSerializer::Serialize(Transaction.Json.ToSharedRef(), Writer);
			OnSuccess(OutputString);
	#if PLATFORM_ANDROID
	NativeOAuth::AndroidLog("CallContractDone");
	#endif
		},GenericFailure);
	}
}

void SequenceAPITest::SendERC20(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API Send[ERC20]Transaction Test]========================"));
	
	TArray<TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>> Txn;
	
	FERC20Transaction T20;
	T20.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	T20.tokenAddress = "0x2791Bca1f2de4661ED88A30C99A7a9449Aa84174";
	T20.value = "1000";
	
	Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T20));
	Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T20));
	Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T20));
	Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T20));

	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		Api->SendTransaction(Txn,[=](FTransactionResponse Transaction)
		{
			FString OutputString;
			const TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
			FJsonSerializer::Serialize(Transaction.Json.ToSharedRef(), Writer);
			UE_LOG(LogTemp,Display,TEXT("Transaction Hash: %s"),*Transaction.TxHash);
			OnSuccess(OutputString);
		},GenericFailure);
	}
}
void SequenceAPITest::SendERC721(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{

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
	T721.tokenAddress = "0xa9a6A3626993D487d2Dbda3173cf58cA1a9D9e9f";
	
	Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T721));

	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		Api->SendTransaction(Txn,[=](FTransactionResponse Transaction)
		{
			FString OutputString;
			TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
			FJsonSerializer::Serialize(Transaction.Json.ToSharedRef(), Writer);
			UE_LOG(LogTemp,Display,TEXT("Transaction Hash: %s"),*Transaction.TxHash);
			OnSuccess(OutputString);
		},GenericFailure);
	}
}
void SequenceAPITest::SendERC1155(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API Send[ERC1155]Transaction Test]========================"));
	
	TArray<TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>> Txn;
	FERC1155Transaction T1155;
	T1155.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	T1155.tokenAddress = "0x631998e91476DA5B870D741192fc5Cbc55F5a52E";
	
	FERC1155TxnValue Val;
	Val.amount = "1";
	Val.id = "66635";
	T1155.vals.Add(Val);

	Txn.Push(TUnion<FRawTransaction,FERC20Transaction,FERC721Transaction,FERC1155Transaction>(T1155));

	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		Api->SendTransaction(Txn,[=](FTransactionResponse Transaction)
		{
			FString OutputString;
			TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
			FJsonSerializer::Serialize(Transaction.Json.ToSharedRef(), Writer);
			UE_LOG(LogTemp,Display,TEXT("Transaction Hash: %s"),*Transaction.TxHash);
			OnSuccess(OutputString);
		},GenericFailure);
	}
}

void SequenceAPITest::CloseSession(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
#if PLATFORM_ANDROID
	NativeOAuth::AndroidLog("CloseSession");
#endif

	const TFunction<void(FString)> OnResponse = [OnSuccess](const FString& Response)
	{
		OnSuccess("CloseSession Test Passed");
#if PLATFORM_ANDROID
		NativeOAuth::AndroidLog("CloseSessionDone");
#endif
	};
	
	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
#if PLATFORM_ANDROID
		NativeOAuth::AndroidLog("CloseSessionFailed");
#endif
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API CloseSession Test]========================"));

	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		Api->CloseSession(OnResponse,GenericFailure);
	}
}