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
	ret.account_id = this->account_id;
	ret.email = this->user_email;
	ret.email_service = this->email_service;
	ret.username = this->username;
	return ret;
}

// Sets default values
ASequenceBackendManager::ASequenceBackendManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->hex_data.Add("0");
	this->hex_data.Add("1");
	this->hex_data.Add("2");
	this->hex_data.Add("3");
	this->hex_data.Add("4");
	this->hex_data.Add("5");
	this->hex_data.Add("6");
	this->hex_data.Add("7");
	this->hex_data.Add("8");
	this->hex_data.Add("9");
	this->hex_data.Add("a");
	this->hex_data.Add("b");
	this->hex_data.Add("c");
	this->hex_data.Add("d");
	this->hex_data.Add("e");
	this->hex_data.Add("f");
	this->sequenceWallet = new SequenceAPI::FSequenceWallet();
	this->Indexer = NewObject<UIndexer>();//for handling indexer data
	this->auth = NewObject<UAuth>();
	this->request_handler = NewObject<UObjectHandler>();//create our handler!
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
void ASequenceBackendManager::Copy_To_Clipboard(FString data)
{
	//rebuild these 2 functions using #ifdefs for platforms IOS, Android, Mac, Windows to clear out this warning!
	//works for now
	FGenericPlatformMisc::ClipboardCopy(*data);
}

FString ASequenceBackendManager::Get_From_Clipboard()
{
	FString retData;
	//gets data from clipboard but it comes back invalid? this will be broken until we move up engine versions (hopefully epic actually implements a real solution)
	FGenericPlatformMisc::ClipboardPaste(retData);
	return retData;
}

FString ASequenceBackendManager::get_transaction_hash(FTransaction_FE Transaction)
{
	return Transaction.ID();
}

FSecureKey ASequenceBackendManager::getSecureStorableAuth()
{
	return this->auth->GetSecureStorableAuth();//get the stored auth data ready for storage!
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
		this->receiveCoin(RCoin);
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

		this->receiveNFT(RNft);
	}
	FTimerHandle TimerTestReceive;
	FTimerDelegate Delegate; // Delegate to bind function with parameters
	Delegate.BindUFunction(this, "randomReceive"); // Character is the parameter we wish to pass with the function.
	GetWorld()->GetTimerManager().SetTimer(TimerTestReceive, Delegate,FMath::FRandRange(1,15), false);
}

void ASequenceBackendManager::UpdateSystemTestableData(const FSystemData_BE& system_data)
{
	this->testCoins = system_data.user_data.coins;
	this->testNFTs = system_data.user_data.nfts;

	//need to setup a special timer function delegate!

	FTimerHandle TimerTestReceive;
	FTimerDelegate Delegate; // Delegate to bind function with parameters
	Delegate.BindUFunction(this, "randomReceive"); // Character is the parameter we wish to pass with the function.
	GetWorld()->GetTimerManager().SetTimer(TimerTestReceive, Delegate, 60, false);

	this->update_system_data(system_data);
}

void ASequenceBackendManager::InitSystemData()
{
	UE_LOG(LogTemp, Display, TEXT("[System Data Fetch INITIATED]"));
	USystemDataBuilder * builder = NewObject<USystemDataBuilder>();
	//Note we still need Auth prior to this but the idea is all of this is already setup and ready to go for this call
	builder->initBuildSystemData(this->sequenceWallet, this->glb_ChainID, this->glb_PublicAddress, this);
}

void ASequenceBackendManager::InitSignin(FString Email)
{
	UE_LOG(LogTemp, Display, TEXT("[Signin Request Initiated]"));//first chunk simulates signin request code gen
	FTimerHandle TH_signin_delay;
	FTimerDelegate Delegate;
	const FString oob_code = "123456";
	Delegate.BindUFunction(this, "update_signin", oob_code);
	GetWorld()->GetTimerManager().SetTimer(TH_signin_delay, Delegate,1, false);

	FTimerHandle TH_auth_delay;//second chunk simulates successful login this may be causing a crash
	FTimerDelegate Delegate_2;
	Delegate_2.BindUFunction(this, "update_authentication", true);
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
		this->update_txn(Callback);
	};

	const TFunction<void (FString, FSequenceError)> SendFailure = [this](const SequenceAPI::TransactionID ID, const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error With Transaction] [%s]"),*Error.Message);
		FTxnCallback_BE Callback;
		Callback.good_txn = false;
		Callback.txn_hash_id = ID;
		this->update_txn(Callback);
	};

	this->sequenceWallet->SendTransactionWithCallback(TransactionData,SendSuccess,SendFailure);
}

void ASequenceBackendManager::init_nft_send_txn(FTransaction_FE TransactionData)
{
	UE_LOG(LogTemp, Display, TEXT("[NFT Txn Request Initiated]"));
	const TSuccessCallback<FString> SendSuccess = [this](const FString ID)
	{
		FTxnCallback_BE Callback;
		Callback.good_txn = true;
		Callback.txn_hash_id = ID;
		this->update_txn(Callback);
	};

	const TFunction<void (FString, FSequenceError)> SendFailure = [this](const FString ID, const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error With Transaction] [%s]"), *Error.Message);
		FTxnCallback_BE callback;
		callback.good_txn = false;
		callback.txn_hash_id = ID;
		this->update_txn(callback);
	};

	this->sequenceWallet->SendTransactionWithCallback(TransactionData, SendSuccess, SendFailure);
}

//update this to be the encrypted json string
void ASequenceBackendManager::InitAuthentication(FSecureKey storedAuthData)
{
	UE_LOG(LogTemp, Display, TEXT("[AUTH INITIATED]"));
	FTimerHandle TH_auth_delay;
	FTimerDelegate Delegate; // Delegate to bind function with parameters
	Delegate.BindUFunction(this, "update_authentication", this->auth->SetSecureStorableAuth(storedAuthData));
	GetWorld()->GetTimerManager().SetTimer(TH_auth_delay, Delegate, FMath::RandRange(1,4), false);
}

void ASequenceBackendManager::InitGetUpdatedCoinData(TArray<FID_BE> CoinsToUpdate)
{	
	UE_LOG(LogTemp, Display, TEXT("[Update Coin Fetch INITIATED]"));
	
	const TSuccessCallback<TArray<FItemPrice_BE>> GenericSuccess = [this](const TArray<FItemPrice_BE> updatedCoinData)
	{
		this->updateCoinData(updatedCoinData);
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error in fetching updated coin data]"));
		TArray<FItemPrice_BE> bList;
		this->updateTokenData(bList);//we need to continue if possible otherwise the frontend hangs
	};

	this->sequenceWallet->getUpdatedCoinPrices(CoinsToUpdate,GenericSuccess,GenericFailure);
}


void ASequenceBackendManager::InitGetUpdateTokenData(TArray<FID_BE> TokensToUpdate)
{
	UE_LOG(LogTemp, Display, TEXT("[Update NFT Fetch INITIATED]"));
	const TSuccessCallback<TArray<FItemPrice_BE>> GenericSuccess = [this](const TArray<FItemPrice_BE> updatedTokenData)
	{
		this->updateTokenData(updatedTokenData);
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error in fetching updated token data]"));
		TArray<FItemPrice_BE> bList;
		this->updateTokenData(bList);//we need to continue if possible otherwise the frontend hangs
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
	FString rep_content = "[error]";
	if (bWasSuccessful)
	{//Parse the json response
		Response = Request.Get()->GetResponse();
		rep_content = Response.Get()->GetContentAsString();//use our rep handler instead!

		TSharedPtr<FJsonObject> json_step;
		if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(rep_content), json_step))//convert to json object!
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to parse into json object: %s: "),*rep_content);
		}
		//we want result & id
		this->recv_block_num = json_step.Get()->GetStringField("result");
		this->recv_id = json_step.Get()->GetIntegerField("id");
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
	if (bWasSuccessful)
		PostJsonRequest(GetMainURL(), CreateHashRequest(this->recv_block_num, this->recv_id), &ASequenceBackendManager::GetHashHandler);
}

void ASequenceBackendManager::GetHashHandler(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FString rep_content = "[error]";
	if (bWasSuccessful)
	{//Parse the json response
		Response = Request.Get()->GetResponse();
		rep_content = Response.Get()->GetContentAsString();//use our rep handler instead!

		TSharedPtr<FJsonObject> json_step;
		if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(rep_content), json_step))//convert to json object!
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to parse into json object: %s: "), *rep_content);
		}

		TSharedPtr<FJsonObject> res_obj = json_step.Get()->GetObjectField("result");
		this->recv_block_hsh = res_obj.Get()->GetStringField("hash");
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
	PostJsonRequest(GetSigninURL(), CreateRequestBody(this->user_email), &ASequenceBackendManager::SigninHandler);
}

//PRIVATE HANDLERS//

FString ASequenceBackendManager::CreateBlkRequest()
{
	return "{\"method\":\"eth_blockNumber\",\"params\":[],\"id\":43,\"jsonrpc\":\"2.0\"}";
}

FString ASequenceBackendManager::CreateHashRequest(FString blk_num,int32 id)
{
	FString ret = "{\"method\":\"eth_getBlockByNumber\",\"params\":[\"";
	ret.Append(blk_num);
	ret.Append("\",false],\"id\":46");
	ret.Append(", \"jsonrpc\":\"2.0\"}");
	UE_LOG(LogTemp, Display, TEXT("Hsh Payload: %s"), *ret);
	return ret;
}

void ASequenceBackendManager::PostJsonRequest(FString url, FString json,void (ASequenceBackendManager::*handler)(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful))
{
	FString response = "[FAILURE]";
	FHttpRequestPtr http_post_req = FHttpModule::Get().CreateRequest();
	http_post_req->SetVerb("POST");
	http_post_req->SetHeader("Content-Type", "application/json");
	http_post_req->SetURL(url);
	http_post_req->SetContentAsString(json);
	http_post_req->OnProcessRequestComplete().BindUObject(this,handler);//here we pass in one of our handler functions to do the processing!
	http_post_req->ProcessRequest();
}

FString ASequenceBackendManager::CreateRequestBody(FString Email)
{
	FString body = "{\"requestType\":\"EMAIL_SIGNIN\",\"email\":";
	FString email_body = "\"";
	email_body.Append(Email);
	email_body.Append("\"");
	body.Append(email_body);
	body.Append(",");
	body.Append("\"clientType\":\"CLIENT_TYPE_WEB\",\"continueUrl\":\"");
	body.Append(GetContinueURL());
	body.Append(CreateIntent(Email));
	body.Append("\",\"canHandleCodeInApp\":true}");
	
	TSharedPtr<FJsonObject> JSON_Step;
	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(body), JSON_Step))
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
	int64 time_in_seconds = date.ToUnixTimestamp();
	Intent.Append(FString::Printf(TEXT("%lld"), time_in_seconds));//allows converting to int64 to FString!
	Intent.Append(",\"blockNumber\":");
	Intent.Append(this->recv_block_num);
	Intent.Append(",\"blockHash\":\"");
	Intent.Append(this->recv_block_hsh);
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
	int64 time_in_seconds = date.ToUnixTimestamp();
	FString lcl_prv_key;
	FString lcl_pblc_key;
	for (int i = 0; i < 64; i++)
	{
		FMath::RandInit(time_in_seconds);//init based on time to get some pseudo random data!
		FString rand_data = this->hex_data[FMath::RandRange(0,this->hex_data.Num()-1)];
		lcl_prv_key.Append(rand_data);
	}

	this->prvt_key = lcl_prv_key;

	auto PrivKey = FPrivateKey::From(lcl_prv_key);//setup the private_key for use!
	FPublicKey gen_pblc_key_H = GetPublicKey(PrivKey);//Gen the public key!
	lcl_pblc_key = gen_pblc_key_H.ToHex();//get the public key in hex form!
	this->pblc_key = lcl_pblc_key;
	return lcl_pblc_key;
}

FString ASequenceBackendManager::GetMainURL()
{
	FString main_url;
	FString result;
	TArray<FString> mn_u;
	mn_u.Add("aHR0");
	mn_u.Add("cHM6Ly9k");
	mn_u.Add("ZXYt");
	mn_u.Add("bm9kZX");
	mn_u.Add("Mu");
	mn_u.Add("c2Vx");
	mn_u.Add("dWVuY2UuYX");
	mn_u.Add("BwL21haW5uZXQ=");

	for (auto i : mn_u)
	{
		main_url.Append(i);
	}
	FBase64::Decode(main_url, result);
	return result;
}

FString ASequenceBackendManager::GetContinueURL()
{
	FString cont_url;
	FString result;
	TArray<FString> ct_u;
	ct_u.Add("aHR0cDo");
	ct_u.Add("vL2xvY2Fs");
	ct_u.Add("aG9zdDozMzM");
	ct_u.Add("zL2F1dG");
	ct_u.Add("g/aW50ZW50PQ==");

	for (auto i : ct_u)
	{
		cont_url.Append(i);
	}

	FBase64::Decode(cont_url, result);
	return result;
}

FString ASequenceBackendManager::GetSigninURL()
{
	FString SigninURL;
	FString Result;
	TArray<FString> sg_u;
	sg_u.Add("aHR0cHM6");
	sg_u.Add("Ly9pZGVu");
	sg_u.Add("dGl0eXRvb2xraXQuZ29vZ");
	sg_u.Add("2xlYXBpcy5jb20");
	sg_u.Add("vdj");
	sg_u.Add("EvY");
	sg_u.Add("WNjb3VudHM6c2VuZE9vYk");
	sg_u.Add("NvZG");
	sg_u.Add("U/a2V5PUFJem");
	sg_u.Add("FTeUN6cEl3W");
	sg_u.Add("GFjeFVISEEyNlB");
	sg_u.Add("PZmdPRnVBc");
	sg_u.Add("2dXc1VMSXVWWQ==");

	for (auto i : sg_u)
	{
		SigninURL.Append(i);
	}

	FBase64::Decode(SigninURL, Result);
	return Result;
}

//end of old signin stuff