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
	
	const TSuccessCallback<FSeqSignMessageResponse_Response> OnResponse = [OnSuccess] (FSeqSignMessageResponse_Response Response)
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
	const TSuccessCallback<TArray<FSeqListSessions_Session>> OnResponse = [OnSuccess](TArray<FSeqListSessions_Session> Response)
	{
		for (auto Session : Response)
		{
			UE_LOG(LogTemp, Display, TEXT("Session: %s"), *USequenceSupport::StructToString(Session));	
		}
		
		OnSuccess("List Sessions Test Passed");
	};
	
	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
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
	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API Send[Raw]Transaction Test]========================"));
	
	TArray<TransactionUnion> Txn;
	FRawTransaction T;
	T.data = "data?";
	T.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	T.value = "0";
	
	Txn.Push(TransactionUnion(T));

	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		Api->SendTransaction(Txn,[=](FSeqTransactionResponse_Data Transaction)
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
	
	
	TArray<TransactionUnion> Txn;
	FRawTransaction T;
	
	T.data = "0x" + EncodedData.ToHex();
	T.to = "0x64d9f9d527abe2a1c1ce3fada98601c4ac5bfdd2";
	T.value = "0";
	
	Txn.Push(TransactionUnion(T));

	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		Api->SendTransaction(Txn,[=](FSeqTransactionResponse_Data Transaction)
		{
			FString OutputString;
			TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
			FJsonSerializer::Serialize(Transaction.Json.ToSharedRef(), Writer);
			OnSuccess(OutputString);
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
	
	TArray<TransactionUnion> Txn;
	
	FERC20Transaction T20;
	T20.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	T20.tokenAddress = "0x2791Bca1f2de4661ED88A30C99A7a9449Aa84174";
	T20.value = "1000";
	
	Txn.Push(TransactionUnion(T20));
	Txn.Push(TransactionUnion(T20));
	Txn.Push(TransactionUnion(T20));
	Txn.Push(TransactionUnion(T20));

	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		Api->SendTransaction(Txn,[=](FSeqTransactionResponse_Data Transaction)
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
	
	TArray<TransactionUnion> Txn;

	FERC721Transaction T721;
	T721.safe = true;
	T721.id = "101424663676543340124133645812717438592241191887187111290563634379068086785120";
	T721.to = "0x245b738089F1fa668D927422d2943F75A9e89819";
	T721.tokenAddress = "0xa9a6a3626993d487d2dbda3173cf58ca1a9d9e9f";
	
	Txn.Push(TransactionUnion(T721));

	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		Api->SendTransaction(Txn,[=](FSeqTransactionResponse_Data Transaction)
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
	
	TArray<TransactionUnion> Txn;
	FERC1155Transaction T1155;
	T1155.to = "0x245b738089F1fa668D927422d2943F75A9e89819";
	T1155.tokenAddress = "0x44b3f42e2bf34f62868ff9e9dab7c2f807ba97cb";
	
	FERC1155TxnValue Val;
	Val.amount = "1";
	Val.id = "130";
	T1155.vals.Add(Val);

	Txn.Push(TransactionUnion(T1155));

	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		Api->SendTransaction(Txn,[=](FSeqTransactionResponse_Data Transaction)
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
	const TFunction<void(FString)> OnResponse = [OnSuccess](const FString& Response)
	{
		OnSuccess("CloseSession Test Passed");
	};
	
	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API SignOut Test]========================"));

	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Api = WalletOptional.GetValue();
		Api->SignOut();
	}
}

void SequenceAPITest::GetFeeOptions(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const TFunction<void(TArray<FFeeOption>)> OnResponse = [OnSuccess](const TArray<FFeeOption>& Response)
	{
		for (const FFeeOption& Fee : Response)
		{
			FString FeeOption = USequenceSupport::StructToString(Fee);
			UE_LOG(LogTemp,Display,TEXT("FeeOption: %s"), *FeeOption);
		}
		OnSuccess("Get FeeOptions Test Passed");
	};

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
	};

	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API GetFeeOptions Test]========================"));

	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();

		TArray<TransactionUnion> Transactions;

		FERC20Transaction T20;
		T20.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
		T20.tokenAddress = "0x2791Bca1f2de4661ED88A30C99A7a9449Aa84174";
		T20.value = "1000";
		Transactions.Push(TransactionUnion(T20));
		
		Api->GetFeeOptions(Transactions,OnResponse,GenericFailure);
	}
}

void SequenceAPITest::SendTransactionWithFee(TFunction<void(FString)> OnSuccess, TFunction<void(FString,FSequenceError)> OnFailure)
{
	TArray<TransactionUnion> Transactions;

	FERC20Transaction T20;
	T20.to = "0x245b738089F1fa668D927422d2943F75A9e89819";
	T20.tokenAddress = "0x2791Bca1f2de4661ED88A30C99A7a9449Aa84174";
	T20.value = "100";

	Transactions.Push(TransactionUnion(T20));
	
	const TFunction<void(TArray<FFeeOption>)> OnFeeResponse = [Transactions, OnSuccess, OnFailure](const TArray<FFeeOption>& Response)
	{
		if (Response.Num() > 0)
		{
			const FFeeOption SelectedFeeOption = Response[0];
			UE_LOG(LogTemp, Display, TEXT("Using FeeOption: %s"), *USequenceSupport::StructToString(SelectedFeeOption));
			
			const FFailureCallback OnTransactionFailure = [OnFailure](const FSequenceError& Error)
			{
				OnFailure("Transaction failure", Error);
			};

			const UAuthenticator * Auth = NewObject<UAuthenticator>();
			const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
			if (WalletOptional.IsSet() && WalletOptional.GetValue())
			{
				USequenceWallet * Api = WalletOptional.GetValue();
				Api->SendTransactionWithFeeOption(Transactions,SelectedFeeOption,[=](const FSeqTransactionResponse_Data& Transaction)
				{
					FString OutputString;
					const TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
					FJsonSerializer::Serialize(Transaction.Json.ToSharedRef(), Writer);
					OnSuccess(OutputString);
				}, OnTransactionFailure);
			}
		}
		else
		{
			OnFailure("Test failed no fee options in response",FSequenceError(EErrorType::EmptyResponse,"Empty fee option response"));
		}
	};

	const FFailureCallback OnFeeFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Get Fee Option Response failure", Error);
	};

	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		Api->GetFeeOptions(Transactions,OnFeeResponse,OnFeeFailure);
	}
}

void SequenceAPITest::GetUnfilteredFeeOptions(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const TFunction<void(TArray<FFeeOption>)> OnResponse = [OnSuccess](const TArray<FFeeOption>& Response)
	{
		for (const FFeeOption& Fee : Response)
		{
			FString FeeOption = USequenceSupport::StructToString(Fee);
			FString CanAfford = (Fee.bCanAfford) ? "Yes" : "No";
			UE_LOG(LogTemp,Display,TEXT("FeeOption: %s\nCanAfford: %s"), *FeeOption, *CanAfford);
			
		}
		OnSuccess("Get FeeOptions Test Passed");
	};

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
	};

	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API GetUnfilteredFeeOptions Test]========================"));

	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();

		TArray<TransactionUnion> Transactions;

		FERC20Transaction T20;
		T20.to = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
		T20.tokenAddress = "0x2791Bca1f2de4661ED88A30C99A7a9449Aa84174";
		T20.value = "1000";
		Transactions.Push(TransactionUnion(T20));
		
		Api->GetUnfilteredFeeOptions(Transactions,OnResponse,GenericFailure);
	}
}

void SequenceAPITest::GetSupportedCountries(const TSuccessCallback<TArray<FSupportedCountry>>& OnSuccess, const FFailureCallback& OnFailure)
{
	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		Api->GetSupportedTransakCountries(OnSuccess,OnFailure);
	}
}

void SequenceAPITest::TestLoadTransakUrl()
{
	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		Api->OpenTransakLink();
	}
}
