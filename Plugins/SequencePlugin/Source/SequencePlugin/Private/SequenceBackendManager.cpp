// Fill out your copyright notice in the Description page of Project Settings.

#include "SequenceBackendManager.h"
#include "Provider.h"
#include "Util/HexUtility.h"
#include "Misc/AutomationTest.h"
#include "GenericPlatform/GenericPlatformApplicationMisc.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Ecdsa.hpp"
#include "Indexer/Indexer.h"
#include "Eth/Crypto.h"
#include "SystemDataBuilder.h"

FUserDetails ASequenceBackendManager::getUserDetails()
{
	FUserDetails ret;
	ret.accountID = this->accountID;
	ret.email = this->userEmail;
	ret.emailService = this->emailService;
	ret.username = this->username;
	return ret;
}

// Sets default values
ASequenceBackendManager::ASequenceBackendManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->hexDataList.Add("0");
	this->hexDataList.Add("1");
	this->hexDataList.Add("2");
	this->hexDataList.Add("3");
	this->hexDataList.Add("4");
	this->hexDataList.Add("5");
	this->hexDataList.Add("6");
	this->hexDataList.Add("7");
	this->hexDataList.Add("8");
	this->hexDataList.Add("9");
	this->hexDataList.Add("a");
	this->hexDataList.Add("b");
	this->hexDataList.Add("c");
	this->hexDataList.Add("d");
	this->hexDataList.Add("e");
	this->hexDataList.Add("f");
	this->sequenceWallet = new SequenceAPI::FSequenceWallet();
	this->Indexer = NewObject<UIndexer>();//for handling indexer data
	this->auth = NewObject<UAuth>();
	this->requestHandler = NewObject<UObjectHandler>();//create our handler!
	this->authenticator = NewObject<UAuthenticator>();
}

ASequenceBackendManager::~ASequenceBackendManager()
{
	if (this->sequenceWallet)
	{
		delete this->sequenceWallet;//free memory only if it's valid!
	}
}

// Called when the game starts or when spawned
void ASequenceBackendManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASequenceBackendManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//SYNC FUNCTIONAL CALLS// [THESE ARE BLOCKING CALLS AND WILL RETURN DATA IMMEDIATELY]

/*
	Used to copy data to the systems clipboard!
*/
void ASequenceBackendManager::CopyToClipboard(FString data)
{
	//rebuild these 2 functions using #ifdefs for platforms IOS, Android, Mac, Windows to clear out this warning!
	//works for now
	FGenericPlatformMisc::ClipboardCopy(*data);
}

FString ASequenceBackendManager::GetFromClipboard()
{
	FString retData;
	//gets data from clipboard but it comes back invalid? this will be broken until we move up engine versions (hopefully epic actually implements a real solution)
	FGenericPlatformMisc::ClipboardPaste(retData);
	return retData;
}

FString ASequenceBackendManager::GetTransactionHash(FTransaction_FE Transaction)
{
	return Transaction.ID();
}

FSecureKey ASequenceBackendManager::GetSecureStorableAuth()
{
	return this->auth->GetSecureStorableAuth();//get the stored auth data ready for storage!
}

FString ASequenceBackendManager::GetLoginURL()
{
	return this->authenticator->GetSigninURL();
}

//SYNC FUNCTIONAL CALLS// [THESE ARE BLOCKING CALLS AND WILL RETURN DATA IMMEDIATELY]

//ASYNC FUNCTIONAL CALLS// [THESE ARE NON BLOCKING CALLS AND WILL USE A MATCHING UPDATE...FUNC TO RETURN DATA]

void ASequenceBackendManager::RandomReceive()
{
	int32 Dist = FMath::RandRange(1, 100);
	int32 AcceptRange = 10;
	bool bGenNew = (Dist <= AcceptRange);//acceptRange % of the time we will generate something new!

	if (FMath::RandBool())
	{//coin
		FCoin_BE RCoin = this->testCoins[FMath::RandRange(0, this->testCoins.Num() - 1)];
		RCoin.Coin_Amount = FMath::FRandRange(0.001, 100);
		if (bGenNew)
		{
			RCoin.Coin_Long_Name = "NewRandomCoin:";
			RCoin.Coin_Long_Name.AppendInt(FMath::RandRange(0,1000));
		}
		this->ReceiveCoin(RCoin);
	}
	else
	{//nft
		FNFT_Master_BE RNft = this->testNFTs[FMath::RandRange(0, this->testNFTs.Num() - 1)];
		
		int32 RCount = FMath::RandRange(1, 100);
		for (int32 i = 0; i < RCount; i++)
		{
			FNFT_UData_BE uData;
			RNft.nft_data.Add(uData);
		}

		if (bGenNew)
		{
			RNft.NFT_Name = "NewRandomNFT";
			RNft.NFT_Name.AppendInt(FMath::RandRange(0, 1000));
			RNft.Collection_Long_Name = "Testing_Collection";
			RNft.Collection_Short_Name = "Tst";
		}

		this->ReceiveNFT(RNft);
	}
	FTimerHandle TimerTestReceive;
	FTimerDelegate Delegate; // Delegate to bind function with parameters
	Delegate.BindUFunction(this, "randomReceive"); // Character is the parameter we wish to pass with the function.
	GetWorld()->GetTimerManager().SetTimer(TimerTestReceive, Delegate,FMath::FRandRange(1,15), false);
}

void ASequenceBackendManager::UpdateSystemTestableData(const FSystemData_BE& systemData)
{
	this->testCoins = systemData.user_data.coins;
	this->testNFTs = systemData.user_data.nfts;

	//need to setup a special timer function delegate!

	FTimerHandle TimerTestReceive;
	FTimerDelegate Delegate; // Delegate to bind function with parameters
	Delegate.BindUFunction(this, "randomReceive"); // Character is the parameter we wish to pass with the function.
	GetWorld()->GetTimerManager().SetTimer(TimerTestReceive, Delegate, 60, false);

	this->UpdateSystemData(systemData);
}

void ASequenceBackendManager::InitSystemData()
{
	UE_LOG(LogTemp, Display, TEXT("[System Data Fetch INITIATED]"));
	USystemDataBuilder * builder = NewObject<USystemDataBuilder>();
	//Note we still need Auth prior to this but the idea is all of this is already setup and ready to go for this call
	builder->initBuildSystemData(this->sequenceWallet, this->chainID, this->publicAddress, this);
}

void ASequenceBackendManager::InitSignin(FString Email)
{
	UE_LOG(LogTemp, Display, TEXT("[Signin Request Initiated]"));//first chunk simulates signin request code gen
	FTimerHandle THSigninDelay;
	FTimerDelegate Delegate;
	const FString oobCode = "123456";
	Delegate.BindUFunction(this, "UpdateSignin", oobCode);
	GetWorld()->GetTimerManager().SetTimer(THSigninDelay, Delegate,1, false);

	FTimerHandle TH_auth_delay;//second chunk simulates successful login this may be causing a crash
	FTimerDelegate Delegate_2;
	Delegate_2.BindUFunction(this, "UpdateAuthentication", true);
	GetWorld()->GetTimerManager().SetTimer(TH_auth_delay, Delegate_2,3, false);
}

void ASequenceBackendManager::InitCoinSendTxn(FTransaction_FE TransactionData)
{
	UE_LOG(LogTemp, Display, TEXT("[Coin Txn Request Initiated]"));//first chunk simulates signin request code gen

	const TSuccessCallback<FString> SendSuccess = [this](const FString ID)
	{
		FTxnCallback_BE Callback;
		Callback.good_txn = true;
		Callback.txn_hash_id = ID;
		this->UpdateTxn(Callback);
	};

	const TFunction<void (FString, FSequenceError)> SendFailure = [this](const SequenceAPI::TransactionID ID, const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error With Transaction] [%s]"),*Error.Message);
		FTxnCallback_BE Callback;
		Callback.good_txn = false;
		Callback.txn_hash_id = ID;
		this->UpdateTxn(Callback);
	};

	this->sequenceWallet->SendTransactionWithCallback(TransactionData,SendSuccess,SendFailure);
}

void ASequenceBackendManager::InitNFTSendTxn(FTransaction_FE TransactionData)
{
	UE_LOG(LogTemp, Display, TEXT("[NFT Txn Request Initiated]"));
	const TSuccessCallback<FString> SendSuccess = [this](const FString ID)
	{
		FTxnCallback_BE Callback;
		Callback.good_txn = true;
		Callback.txn_hash_id = ID;
		this->UpdateTxn(Callback);
	};

	const TFunction<void (FString, FSequenceError)> SendFailure = [this](const FString ID, const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error With Transaction] [%s]"), *Error.Message);
		FTxnCallback_BE callback;
		callback.good_txn = false;
		callback.txn_hash_id = ID;
		this->UpdateTxn(callback);
	};

	this->sequenceWallet->SendTransactionWithCallback(TransactionData, SendSuccess, SendFailure);
}

//update this to be the encrypted json string
void ASequenceBackendManager::InitAuthentication(FSecureKey storedAuthData)
{
	UE_LOG(LogTemp, Display, TEXT("[AUTH INITIATED]"));
	FTimerHandle TH_auth_delay;
	FTimerDelegate Delegate; // Delegate to bind function with parameters
	Delegate.BindUFunction(this, "UpdateAuthentication", this->auth->SetSecureStorableAuth(storedAuthData));
	GetWorld()->GetTimerManager().SetTimer(TH_auth_delay, Delegate, FMath::RandRange(1,4), false);
}

void ASequenceBackendManager::InitGetUpdatedCoinData(TArray<FID_BE> CoinsToUpdate)
{	
	UE_LOG(LogTemp, Display, TEXT("[Update Coin Fetch INITIATED]"));
	
	const TSuccessCallback<TArray<FItemPrice_BE>> GenericSuccess = [this](const TArray<FItemPrice_BE> updatedCoinData)
	{
		this->UpdateCoinData(updatedCoinData);
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error in fetching updated coin data]"));
		TArray<FItemPrice_BE> bList;
		this->UpdateTokenData(bList);//we need to continue if possible otherwise the frontend hangs
	};

	this->sequenceWallet->getUpdatedCoinPrices(CoinsToUpdate,GenericSuccess,GenericFailure);
}


void ASequenceBackendManager::InitGetUpdateTokenData(TArray<FID_BE> TokensToUpdate)
{
	UE_LOG(LogTemp, Display, TEXT("[Update NFT Fetch INITIATED]"));
	const TSuccessCallback<TArray<FItemPrice_BE>> GenericSuccess = [this](const TArray<FItemPrice_BE> updatedTokenData)
	{
		this->UpdateTokenData(updatedTokenData);
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error in fetching updated token data]"));
		TArray<FItemPrice_BE> bList;
		this->UpdateTokenData(bList);//we need to continue if possible otherwise the frontend hangs
	};

	this->sequenceWallet->getUpdatedCollectiblePrices(TokensToUpdate, GenericSuccess, GenericFailure);
}

//ASYNC FUNCTIONAL CALLS// [THESE ARE NON BLOCKING CALLS AND WILL USE A MATCHING UPDATE...FUNC TO RETURN DATA]

//PRIVATE HANDLERS//

/*
* Signin is pending but I'm suspecting I will create a series of static signin function in a new 
* signin Handler Object to clean this process up!
*/
void ASequenceBackendManager::SigninHandler(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FString rep_content = "[error]";
	if (bWasSuccessful)
	{//Parse the json response
		Response = Request.Get()->GetResponse();
		rep_content = Response.Get()->GetContentAsString();//use our rep handler instead!
	}
	else
	{
		switch (Request->GetStatus()) {
		case EHttpRequestStatus::Failed_ConnectionError:
			UE_LOG(LogTemp, Error, TEXT("Connection failed."));
		default:
			UE_LOG(LogTemp, Error, TEXT("Request failed."));
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Response: %s"), *rep_content);
}

void ASequenceBackendManager::GetBlkHandler(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FString repContent = "[error]";
	if (bWasSuccessful)
	{//Parse the json response
		Response = Request.Get()->GetResponse();
		repContent = Response.Get()->GetContentAsString();//use our rep handler instead!

		TSharedPtr<FJsonObject> jsonStep;
		if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(repContent), jsonStep))//convert to json object!
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to parse into json object: %s: "),*repContent);
		}
		//we want result & id
		this->receiveBlockNumber = jsonStep.Get()->GetStringField("result");
		this->receiveID = jsonStep.Get()->GetIntegerField("id");
	}
	else
	{
		switch (Request->GetStatus()) {
		case EHttpRequestStatus::Failed_ConnectionError:
			UE_LOG(LogTemp, Error, TEXT("Connection failed."));
		default:
			UE_LOG(LogTemp, Error, TEXT("Request failed."));
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Response: %s"), *repContent);
	if (bWasSuccessful)
		PostJsonRequest(GetMainURL(), CreateHashRequest(this->receiveBlockNumber, this->receiveID), &ASequenceBackendManager::GetHashHandler);
}

void ASequenceBackendManager::GetHashHandler(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FString repContent = "[error]";
	if (bWasSuccessful)
	{//Parse the json response
		Response = Request.Get()->GetResponse();
		repContent = Response.Get()->GetContentAsString();//use our rep handler instead!

		TSharedPtr<FJsonObject> jsonStep;
		if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(repContent), jsonStep))//convert to json object!
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to parse into json object: %s: "), *repContent);
		}

		TSharedPtr<FJsonObject> resObj = jsonStep.Get()->GetObjectField("result");
		this->receiveBlockHash = resObj.Get()->GetStringField("hash");
	}
	else
	{
		switch (Request->GetStatus()) {
		case EHttpRequestStatus::Failed_ConnectionError:
			UE_LOG(LogTemp, Error, TEXT("Connection failed."));
		default:
			UE_LOG(LogTemp, Error, TEXT("Request failed."));
		}
	}

	UE_LOG(LogTemp, Display, TEXT("Response: %s"), *repContent);
	PostJsonRequest(GetSigninURL(), CreateRequestBody(this->userEmail), &ASequenceBackendManager::SigninHandler);
}

//PRIVATE HANDLERS//

FString ASequenceBackendManager::CreateBlkRequest()
{
	return "{\"method\":\"eth_blockNumber\",\"params\":[],\"id\":43,\"jsonrpc\":\"2.0\"}";
}

FString ASequenceBackendManager::CreateHashRequest(FString blockNumber,int32 id)
{
	FString ret = "{\"method\":\"eth_getBlockByNumber\",\"params\":[\"";
	ret.Append(blockNumber);
	ret.Append("\",false],\"id\":46");
	ret.Append(", \"jsonrpc\":\"2.0\"}");
	UE_LOG(LogTemp, Display, TEXT("Hsh Payload: %s"), *ret);
	return ret;
}

void ASequenceBackendManager::PostJsonRequest(FString url, FString json,void (ASequenceBackendManager::*handler)(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful))
{
	FString response = "[FAILURE]";
	FHttpRequestPtr httpPostReq = FHttpModule::Get().CreateRequest();
	httpPostReq->SetVerb("POST");
	httpPostReq->SetHeader("Content-Type", "application/json");
	httpPostReq->SetURL(url);
	httpPostReq->SetContentAsString(json);
	httpPostReq->OnProcessRequestComplete().BindUObject(this,handler);//here we pass in one of our handler functions to do the processing!
	httpPostReq->ProcessRequest();
}

FString ASequenceBackendManager::CreateRequestBody(FString Email)
{
	FString body = "{\"requestType\":\"EMAIL_SIGNIN\",\"email\":";
	FString emailBody = "\"";
	emailBody.Append(Email);
	emailBody.Append("\"");
	body.Append(emailBody);
	body.Append(",");
	body.Append("\"clientType\":\"CLIENT_TYPE_WEB\",\"continueUrl\":\"");
	body.Append(GetContinueURL());
	body.Append(CreateIntent(Email));
	body.Append("\",\"canHandleCodeInApp\":true}");
	
	TSharedPtr<FJsonObject> jsonStep;
	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(body), jsonStep))
	{//used for making sure my json in makes sense!
		UE_LOG(LogTemp, Display, TEXT("Failed to parse to json!"));
	}
	UE_LOG(LogTemp, Display, TEXT("Json Body: %s"), *body);
	return body;
}

FString ASequenceBackendManager::CreateIntent(FString Email)
{
	FString Intent = "{\"email\":";
	Intent.Append(Email);
	Intent.Append(",\"sessionSignerAddress\":");
	Intent.Append("\"");
	Intent.Append(SetupRandomWallet());
	Intent.Append("\"");
	//append session signer addr we need to generate a random public key and store it here!
	Intent.Append(",\"timestamp\":");
	FDateTime date = FDateTime::Now();
	int64 timeInSeconds = date.ToUnixTimestamp();
	Intent.Append(FString::Printf(TEXT("%lld"), timeInSeconds));//allows converting to int64 to FString!
	Intent.Append(",\"blockNumber\":");
	Intent.Append(this->receiveBlockNumber);
	Intent.Append(",\"blockHash\":\"");
	Intent.Append(this->receiveBlockHash);
	Intent.Append("\",\"deviceinfo\":{\"broswer\":\"chrome\",\"os\":\"windows\"}");
	Intent.Append(",\"reLogin?\":false,\"testnet?\":false,\"appName?\":\"n/a\",\"origin?\":\"n/a\",\"bannerUrl?\":\"n/a\"}");

	//the last thing I need to do for intent is encode it in base64! then send it out!
	Intent = FBase64::Encode(Intent);

	UE_LOG(LogTemp, Display, TEXT("B64 intent: %s"),*Intent);

	return Intent;
}

FString ASequenceBackendManager::SetupRandomWallet()
{
	//need to generate a random wallet!
	FDateTime date = FDateTime::Now();
	int64 timeInSeconds = date.ToUnixTimestamp();
	FString lcl_prv_key;
	FString lcl_pblc_key;
	for (int i = 0; i < 64; i++)
	{
		FMath::RandInit(timeInSeconds);//init based on time to get some pseudo random data!
		FString rand_data = this->hexDataList[FMath::RandRange(0,this->hexDataList.Num()-1)];
		lcl_prv_key.Append(rand_data);
	}

	this->privateKey = lcl_prv_key;

	auto PrivKey = FPrivateKey::From(lcl_prv_key);//setup the private_key for use!
	FPublicKey gen_pblc_key_H = GetPublicKey(PrivKey);//Gen the public key!
	lcl_pblc_key = gen_pblc_key_H.ToHex();//get the public key in hex form!
	this->publicKey = lcl_pblc_key;
	return lcl_pblc_key;
}

FString ASequenceBackendManager::GetMainURL()
{
	FString mainURL;
	FString result;
	TArray<FString> mnU;
	mnU.Add("aHR0");
	mnU.Add("cHM6Ly9k");
	mnU.Add("ZXYt");
	mnU.Add("bm9kZX");
	mnU.Add("Mu");
	mnU.Add("c2Vx");
	mnU.Add("dWVuY2UuYX");
	mnU.Add("BwL21haW5uZXQ=");

	for (auto i : mnU)
	{
		mainURL.Append(i);
	}
	FBase64::Decode(mainURL, result);
	return result;
}

FString ASequenceBackendManager::GetContinueURL()
{
	FString contURL;
	FString result;
	TArray<FString> ctU;
	ctU.Add("aHR0cDo");
	ctU.Add("vL2xvY2Fs");
	ctU.Add("aG9zdDozMzM");
	ctU.Add("zL2F1dG");
	ctU.Add("g/aW50ZW50PQ==");

	for (auto i : ctU)
	{
		contURL.Append(i);
	}

	FBase64::Decode(contURL, result);
	return result;
}

FString ASequenceBackendManager::GetSigninURL()
{
	FString SigninURL;
	FString Result;
	TArray<FString> sgU;
	sgU.Add("aHR0cHM6");
	sgU.Add("Ly9pZGVu");
	sgU.Add("dGl0eXRvb2xraXQuZ29vZ");
	sgU.Add("2xlYXBpcy5jb20");
	sgU.Add("vdj");
	sgU.Add("EvY");
	sgU.Add("WNjb3VudHM6c2VuZE9vYk");
	sgU.Add("NvZG");
	sgU.Add("U/a2V5PUFJem");
	sgU.Add("FTeUN6cEl3W");
	sgU.Add("GFjeFVISEEyNlB");
	sgU.Add("PZmdPRnVBc");
	sgU.Add("2dXc1VMSXVWWQ==");

	for (auto i : sgU)
	{
		SigninURL.Append(i);
	}

	FBase64::Decode(SigninURL, Result);
	return Result;
}

//end of old signin stuff