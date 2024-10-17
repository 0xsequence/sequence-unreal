// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "GeneralTesting.h"
#include "Tests/IndexerTests.h"
#include "ObjectHandler.h"
#include "Misc/AES.h"
#include "Containers/UnrealString.h"
#include "Util/SequenceSupport.h"
#include "SequenceEncryptor.h"
#include "Sequence/SequenceAPI.h"
#include "Tests/TestSequenceAPI.h"
#include "SequenceAuthenticator.h"

// Sets default values
AGeneralTesting::AGeneralTesting()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	imgHandler = nullptr;
}

// Called when the game starts or when spawned
void AGeneralTesting::BeginPlay()
{
	Super::BeginPlay();
}

void AGeneralTesting::TestSignMessage() const
{
	const TFunction<void (FString)> OnSuccess = [this](FString State)
	{
		CallbackPassed(State);
	};

	const TFunction<void (FString, FSequenceError)> OnFailure = [this](FString Data, FSequenceError Err)
	{
		CallbackFailed(Data, Err);
	};
	
	SequenceAPITest::SignMessage(OnSuccess, OnFailure);
}

void AGeneralTesting::TestListSessions() const
{
	const TFunction<void (FString)> OnSuccess = [this](FString State)
	{
		CallbackPassed(State);
	};

	const TFunction<void (FString, FSequenceError)> OnFailure = [this](FString Data, FSequenceError Err)
	{
		CallbackFailed(Data, Err);
	};
	
	SequenceAPITest::ListSessions(OnSuccess, OnFailure);
}

void AGeneralTesting::TestSendRaw() const
{
	const TFunction<void (FString)> OnSuccess = [this](FString State)
	{
		CallbackPassed(State);
	};

	const TFunction<void (FString, FSequenceError)> OnFailure = [this](FString Data, FSequenceError Err)
	{
		CallbackFailed(Data, Err);
	};
	
	SequenceAPITest::SendRaw(OnSuccess, OnFailure);
}

void AGeneralTesting::TestCallContract() const
{
	const TFunction<void (FString)> OnSuccess = [this](FString State)
	{
		CallbackPassed(State);
	};

	const TFunction<void (FString, FSequenceError)> OnFailure = [this](FString Data, FSequenceError Err)
	{
		CallbackFailed(Data, Err);
	};
	
	SequenceAPITest::CallContract(OnSuccess, OnFailure);
}

void AGeneralTesting::TestSendERC20() const
{
	const TFunction<void (FString)> OnSuccess = [this](FString State)
	{
		CallbackPassed(State);
	};

	const TFunction<void (FString, FSequenceError)> OnFailure = [this](FString Data, FSequenceError Err)
	{
		CallbackFailed(Data, Err);
	};
	
	SequenceAPITest::SendERC20(OnSuccess, OnFailure);
}

void AGeneralTesting::TestSendERC721() const
{
	const TFunction<void (FString)> OnSuccess = [this](FString State)
	{
		CallbackPassed(State);
	};

	const TFunction<void (FString, FSequenceError)> OnFailure = [this](FString Data, FSequenceError Err)
	{
		CallbackFailed(Data, Err);
	};
	
	SequenceAPITest::SendERC721(OnSuccess, OnFailure);
}

void AGeneralTesting::TestSendERC1155() const
{
	const TFunction<void (FString)> OnSuccess = [this](FString State)
	{
		CallbackPassed(State);
	};

	const TFunction<void (FString, FSequenceError)> OnFailure = [this](FString Data, FSequenceError Err)
	{
		CallbackFailed(Data, Err);
	};
	
	SequenceAPITest::SendERC1155(OnSuccess, OnFailure);
}

void AGeneralTesting::TestSendTransactionWithFee() const
{
	const TFunction<void (FString)> OnSuccess = [this](const FString& State)
	{
		CallbackPassed(State);
	};

	const TFunction<void (FString, FSequenceError)> OnFailure = [this](const FString& Data, const FSequenceError& Err)
	{
		CallbackFailed(Data, Err);
	};
	
	SequenceAPITest::SendTransactionWithFee(OnSuccess, OnFailure);
}

void AGeneralTesting::TestGetUnFilteredFeeOptions() const
{
	const TFunction<void (FString)> OnSuccess = [this](FString State)
	{
		CallbackPassed(State);
	};

	const TFunction<void (FString, FSequenceError)> OnFailure = [this](FString Data, FSequenceError Err)
	{
		CallbackFailed(Data, Err);
	};
	
	SequenceAPITest::GetUnfilteredFeeOptions(OnSuccess, OnFailure);
}

void AGeneralTesting::TestGetFeeOptions() const
{
	const TFunction<void (FString)> OnSuccess = [this](FString State)
	{
		CallbackPassed(State);
	};

	const TFunction<void (FString, FSequenceError)> OnFailure = [this](FString Data, FSequenceError Err)
	{
		CallbackFailed(Data, Err);
	};
	
	SequenceAPITest::GetFeeOptions(OnSuccess, OnFailure);
}

void AGeneralTesting::TestCloseSessions() const
{
	const TFunction<void (FString)> OnSuccess = [this](FString State)
	{
		CallbackPassed(State);
	};

	const TFunction<void (FString, FSequenceError)> OnFailure = [this](FString Data, FSequenceError Err)
	{
		CallbackFailed(Data, Err);
	};
	
	SequenceAPITest::CloseSession(OnSuccess, OnFailure);
}

void AGeneralTesting::TestGetSupportedCountries() const
{
	const TFunction<void (TArray<FSupportedCountry>)> OnSuccess = [this](TArray<FSupportedCountry> SupportedCountries)
	{
		for (FSupportedCountry Country : SupportedCountries)
		{
			UE_LOG(LogTemp, Display, TEXT("Country: %s"), *USequenceSupport::StructToString(Country));
		}
		CallbackPassed("TestPassed");
	};

	const TFunction<void (FSequenceError)> OnFailure = [this]( const FSequenceError& Err)
	{
		CallbackFailed(TEXT("Error getting TransakCountry Data"), Err);
	};

	SequenceAPITest::GetSupportedCountries(OnSuccess,OnFailure);
}

void AGeneralTesting::TestLoadTransakUrl() const
{
	SequenceAPITest::TestLoadTransakUrl();
}

void AGeneralTesting::TestGuestLogin()
{
	USequenceAuthenticator * Auth = NewObject<USequenceAuthenticator>();
	Auth->GuestLogin(false);
}

void AGeneralTesting::TestIndexer()
{
	TFunction<void(FString)> OnSuccess = [this](FString State)
	{
		CallbackPassed(State);
	};

	TFunction<void(FString, FSequenceError)> OnFailure = [this](FString data, FSequenceError Err)
	{
		CallbackFailed(data, Err);
	};

	IndexerTest(OnSuccess, OnFailure);
}

void AGeneralTesting::TestTokenBalances() const
{
	const TSuccessCallback<FSeqGetTokenBalancesReturn> GenericSuccess = [](const FSeqGetTokenBalancesReturn tokenBalances)
	{
		const FString ret = USequenceSupport::StructToString<FSeqGetTokenBalancesReturn>(tokenBalances);
		UE_LOG(LogTemp, Display, TEXT("Parsed TokenBalancesReturn Struct:\n%s\n"), *ret);
	};

	const FFailureCallback GenericFailure = [](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("Error with getting balances"));
	};
	
	const USequenceAuthenticator * Auth = NewObject<USequenceAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		FSeqGetTokenBalancesArgs args;
		args.accountAddress = Api->GetWalletAddress();
		args.includeMetaData = true;
		Api->GetTokenBalances(args,GenericSuccess,GenericFailure);
	}
}

void AGeneralTesting::TestHistory() const
{	
	const TSuccessCallback<FSeqGetTransactionHistoryReturn> GenericSuccess = [](const FSeqGetTransactionHistoryReturn transactionHistory)
	{
		if (GPrintAll)
		{
			const FString ret = USequenceSupport::StructToString<FSeqGetTransactionHistoryReturn>(transactionHistory);
			UE_LOG(LogTemp, Display, TEXT("Parsed transactionHistoryReturn Struct:\n%s\n"), *ret);
		}
	};

	const FFailureCallback GenericFailure = [](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Display, TEXT("Error with getting history"));
	};
	
	const USequenceAuthenticator * Auth = NewObject<USequenceAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();
		FSeqGetTransactionHistoryArgs args;
		args.filter.accountAddress = Api->GetWalletAddress();
		args.includeMetaData = true;
		args.page->page = 0;
		args.page->more = true;
		Api->GetTransactionHistory(args,GenericSuccess,GenericFailure);
	}
}

void AGeneralTesting::TestGetWalletAddress() const
{
	const USequenceAuthenticator * Auth = NewObject<USequenceAuthenticator>();
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Auth->GetStoredCredentials().GetCredentials());
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		const USequenceWallet * Api = WalletOptional.GetValue();
		UE_LOG(LogTemp, Display, TEXT("Wallet Address: %s"), *Api->GetWalletAddress());
	}
}

void AGeneralTesting::TestEncryption() const
{
	const FString PreEncrypt = "testing text";
	const FString EncryptedData = USequenceEncryptor::Encrypt(PreEncrypt);
	const FString DecryptedData = USequenceEncryptor::Decrypt(EncryptedData,PreEncrypt.Len());

	UE_LOG(LogTemp, Display, TEXT("Pre Encrypt: %s"), *PreEncrypt);
	UE_LOG(LogTemp, Display, TEXT("Encrypted: %s"), *EncryptedData);
	UE_LOG(LogTemp, Display, TEXT("Post Encrypt: %s"), *DecryptedData);
	const FString TestState = (PreEncrypt.Compare(DecryptedData) == 0) ? "Passed" : "Failed";
	UE_LOG(LogTemp, Display, TEXT("Test: [%s]"), *TestState);
}

void AGeneralTesting::TestMisc()
{//used for testing various things in the engine to verify behaviour
	USequenceAuthenticator * Authenticator = NewObject<USequenceAuthenticator>();
	//Authenticator->PlayFabRegisterAndLogin(TEXT("TBPTest1"), TEXT("calvinvermeulen18@gmail.com"), TEXT("Ac-123Bd-456#!ef"));
	Authenticator->PlayFabLogin(TEXT("TBPTest1"), TEXT("Ac-123Bd-456#!ef"), false);
	//Authenticator->GuestLogin(false);
}

void AGeneralTesting::OnDoneImageProcessing()
{//forward this to the front as we will be able to view all the images from there!
	this->testMiscForwarder(this->imgHandler->GetProcessedImages());
}

// Called every frame
void AGeneralTesting::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGeneralTesting::CallbackPassed(FString StateData) const
{
	UE_LOG(LogTemp, Display, TEXT("========================================================================="));
	UE_LOG(LogTemp, Display, TEXT("[Callback Passed!]\nAdditional State: [%s]"), *StateData);
	UE_LOG(LogTemp, Display, TEXT("========================================================================="));
}

void AGeneralTesting::CallbackFailed(const FString StateData, FSequenceError Error) const
{
	UE_LOG(LogTemp, Display, TEXT("========================================================================="));
	UE_LOG(LogTemp, Error, TEXT("[Callback Failed!]\nAdditional State: [%s]"), *StateData);
	UE_LOG(LogTemp, Error, TEXT("[Error Message]:\n[%s]"),*Error.Message);
	UE_LOG(LogTemp, Error, TEXT("[Error Type]: [%s]"),*ErrorToString(Error.Type));
	UE_LOG(LogTemp, Display, TEXT("========================================================================="));
}

FString AGeneralTesting::ErrorToString(EErrorType Error)
{
	switch (Error) {
	case NotFound:
		return "NotFound";
	case ResponseParseError:
		return "ResponseParseError";
	case EmptyResponse:
		return "EmptyResponse";
	case UnsupportedMethodOnChain:
		return "UnsupportedMethodOnChain";
	case RequestFail:
		return "RequestFail";
	case RequestTimeExceeded:
		return "RequestTimeExceeded";
	case TestFail:
		return "TestFail";
	default:
		return "SequenceError";
	}
}