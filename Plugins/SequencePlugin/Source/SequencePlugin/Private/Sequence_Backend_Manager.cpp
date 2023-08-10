// Fill out your copyright notice in the Description page of Project Settings.

#include "Sequence_Backend_Manager.h"
#include "Provider.h"
#include "HexUtility.h"
#include "Misc/AutomationTest.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Ecdsa.hpp"
#include "Indexer.h"
#include "Crypto.h"

// Sets default values
ASequence_Backend_Manager::ASequence_Backend_Manager()
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
	this->sequence = NewObject<USequenceData>();//for handling sequence data
	this->indexer = NewObject<UIndexer>();//for handling indexer data
	this->request_handler = NewObject<UObjectHandler>();//create our handler!
}

// Called when the game starts or when spawned
void ASequence_Backend_Manager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASequence_Backend_Manager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//SYNC FUNCTIONAL CALLS// [THESE ARE BLOCKING CALLS AND WILL RETURN DATA IMMEDIATELY]

/*
	Used to copy data to the systems clipboard!
*/
void ASequence_Backend_Manager::Copy_To_Clipboard(FString data)
{
	FPlatformMisc::ClipboardCopy(*data);//deprecated call :( , but actually works lol
	//the replacement call isn't even available to use yet ;-; Requires platform lumin which doesn't exist?

	//this function call is straight up not implemented :(
	//FGenericPlatformApplicationMisc::ClipboardCopy(*data);
}

FString ASequence_Backend_Manager::Get_From_Clipboard()
{
	FString* ret;
	FString ret_data;
	ret = &ret_data;
	FPlatformMisc::ClipboardPaste(*ret);//get data from the clip board!
	//again platform misc is deprecated :( but until the generic platform clipboard functions are
	//actually implemented this is the best I can do for the time being.
	return ret_data;
}

FString ASequence_Backend_Manager::get_transaction_hash()
{
	FString txn_hash = "0x";
	int32 txn_value = FMath::RandRange(1, 65536);
	txn_hash.AppendInt(txn_value);
	return txn_hash;
}

//SYNC FUNCTIONAL CALLS// [THESE ARE BLOCKING CALLS AND WILL RETURN DATA IMMEDIATELY]

//ASYNC FUNCTIONAL CALLS// [THESE ARE NON BLOCKING CALLS AND WILL USE A MATCHING UPDATE...FUNC TO RETURN DATA]

void ASequence_Backend_Manager::init_system_data()
{
	UE_LOG(LogTemp, Display, TEXT("[System Data Fetch INITIATED]"));
	this->update_system_data(FSystemData_BE());
}

void ASequence_Backend_Manager::init_signin(FString email)
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

void ASequence_Backend_Manager::init_coin_send_txn(FCoin_Send_Txn_BE coin_txn)
{
	//dummy function for right now we just call back the update_txn with some pseudo random state!
	UE_LOG(LogTemp, Display, TEXT("[Coin Txn Request Initiated]"));//first chunk simulates signin request code gen
	FTxnCallback_BE callback;
	callback.good_txn = FMath::RandBool();
	callback.txn_hash_id = coin_txn.txn_hash_id;

	FTimerHandle TH_auth_delay;
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "update_txn", callback);
	GetWorld()->GetTimerManager().SetTimer(TH_auth_delay, Delegate, FMath::RandRange(1, 30), false);
}

void ASequence_Backend_Manager::init_nft_send_txn(FNFT_Send_Txn_BE nft_txn)
{
	//dummy function for right now we just call back the update_txn with some pseudo random state!
	UE_LOG(LogTemp, Display, TEXT("[NFT Txn Request Initiated]"));//first chunk simulates signin request code gen
	FTxnCallback_BE callback;
	callback.good_txn = FMath::RandBool();
	callback.txn_hash_id = nft_txn.txn_hash_id;

	FTimerHandle TH_auth_delay;
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "update_txn", callback);
	GetWorld()->GetTimerManager().SetTimer(TH_auth_delay, Delegate, FMath::RandRange(1, 30), false);

}

void ASequence_Backend_Manager::testing_network_infrastructures()
{
	TSharedRef<IHttpRequest> http_post_req = FHttpModule::Get().CreateRequest();

	http_post_req->SetVerb("POST");
	http_post_req->SetHeader("Content-Type", "application/json");//2 differing headers for the request
	http_post_req->SetHeader("Accept", "application/json");
	http_post_req->SetURL("https://www.google.ca");
	http_post_req->SetContentAsString("{ some bad json }");//args will need to be a json object converted to a string
	
	http_post_req.Get().OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr Req, FHttpResponsePtr Response, bool IsSuccessful)
		{
			UE_LOG(LogTemp, Display, TEXT("Lambda called!"));
			if (IsSuccessful)
			{
				UE_LOG(LogTemp, Display, TEXT("Success!"));
			}
			else
			{
				UE_LOG(LogTemp, Display, TEXT("Failed!"));
			}
		});



	http_post_req->ProcessRequest();
}

void ASequence_Backend_Manager::init_authentication(FStoredState_BE stored_state)
{
	UE_LOG(LogTemp, Display, TEXT("[AUTH INITIATED]"));
	FTimerHandle TH_auth_delay;
	FTimerDelegate Delegate; // Delegate to bind function with parameters
	Delegate.BindUFunction(this, "update_authentication", true);
	GetWorld()->GetTimerManager().SetTimer(TH_auth_delay, Delegate, FMath::RandRange(1,4), false);
}

void ASequence_Backend_Manager::init_get_updated_coin_data(TArray<FID_BE> coinsToUpdate)
{
	const TSuccessCallback<TArray<FItemPrice_BE>> GenericSuccess = [this](const TArray<FItemPrice_BE> updatedCoinData)
	{
		this->updateCoinData(updatedCoinData);
	};

	const FFailureCallback GenericFailure = [=](const SequenceError Error)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error in fetching updated coin data]"));
	};

	this->sequence->getUpdatedItemPrices(coinsToUpdate,GenericSuccess,GenericFailure);
}


void ASequence_Backend_Manager::init_get_updated_token_data(TArray<FID_BE> tokensToUpdate)
{
	const TSuccessCallback<TArray<FItemPrice_BE>> GenericSuccess = [this](const TArray<FItemPrice_BE> updatedTokenData)
	{
		this->updateTokenData(updatedTokenData);
	};

	const FFailureCallback GenericFailure = [=](const SequenceError Error)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error in fetching updated token data]"));
	};

	this->sequence->getUpdatedItemPrices(tokensToUpdate, GenericSuccess, GenericFailure);
}

void ASequence_Backend_Manager::init_get_updated_fee_data()
{
	//stub in we don't know where we get fee's from yet!
	TArray<FFee_BE> feeData;
	FFee_BE testFee;
	testFee.fee.Coin_Amount = 10;
	feeData.Add(testFee);
	this->updateFeeData(feeData);
}

//ASYNC FUNCTIONAL CALLS// [THESE ARE NON BLOCKING CALLS AND WILL USE A MATCHING UPDATE...FUNC TO RETURN DATA]

//PRIVATE HANDLERS//

void ASequence_Backend_Manager::signin_handler(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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

void ASequence_Backend_Manager::get_blk_handler(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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
		post_json_request(get_main_url(), create_hsh_req(this->recv_block_num, this->recv_id), &ASequence_Backend_Manager::get_hsh_handler);
}

void ASequence_Backend_Manager::get_hsh_handler(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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
	post_json_request(get_signin_url(), create_req_body(this->user_email), &ASequence_Backend_Manager::signin_handler);
}

//PRIVATE HANDLERS//

FString ASequence_Backend_Manager::create_blk_req()
{
	return "{\"method\":\"eth_blockNumber\",\"params\":[],\"id\":43,\"jsonrpc\":\"2.0\"}";
}

FString ASequence_Backend_Manager::create_hsh_req(FString blk_num,int32 id)
{
	FString ret = "{\"method\":\"eth_getBlockByNumber\",\"params\":[\"";
	ret.Append(blk_num);
	ret.Append("\",false],\"id\":46");
	ret.Append(", \"jsonrpc\":\"2.0\"}");
	UE_LOG(LogTemp, Display, TEXT("Hsh Payload: %s"), *ret);
	return ret;
}

void ASequence_Backend_Manager::post_json_request(FString url, FString json,void (ASequence_Backend_Manager::*handler)(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful))
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

FString ASequence_Backend_Manager::create_req_body(FString email)
{
	FString body = "{\"requestType\":\"EMAIL_SIGNIN\",\"email\":";
	FString email_body = "\"";
	email_body.Append(email);
	email_body.Append("\"");
	body.Append(email_body);
	body.Append(",");
	body.Append("\"clientType\":\"CLIENT_TYPE_WEB\",\"continueUrl\":\"");
	body.Append(get_continue_url());
	body.Append(create_intent(email));
	body.Append("\",\"canHandleCodeInApp\":true}");
	
	TSharedPtr<FJsonObject> json_step;
	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(body), json_step))
	{//used for making sure my json in makes sense!
		UE_LOG(LogTemp, Display, TEXT("Failed to parse to json!"));
	}
	UE_LOG(LogTemp, Display, TEXT("Json Body: %s"), *body);
	return body;
}

FString ASequence_Backend_Manager::create_intent(FString email)
{
	FString intent = "{\"email\":";
	intent.Append(email);
	intent.Append(",\"sessionSignerAddress\":");
	intent.Append("\"");
	intent.Append(setup_random_wallet());
	intent.Append("\"");
	//append session signer addr we need to generate a random public key and store it here!
	intent.Append(",\"timestamp\":");
	FDateTime date = FDateTime::Now();
	int64 time_in_seconds = date.ToUnixTimestamp();
	intent.Append(FString::Printf(TEXT("%lld"), time_in_seconds));//allows converting to int64 to FString!
	intent.Append(",\"blockNumber\":");
	intent.Append(this->recv_block_num);
	intent.Append(",\"blockHash\":\"");
	intent.Append(this->recv_block_hsh);
	intent.Append("\",\"deviceinfo\":{\"broswer\":\"chrome\",\"os\":\"windows\"}");
	intent.Append(",\"reLogin?\":false,\"testnet?\":false,\"appName?\":\"n/a\",\"origin?\":\"n/a\",\"bannerUrl?\":\"n/a\"}");

	//the last thing I need to do for intent is encode it in base64! then send it out!
	intent = FBase64::Encode(intent);

	UE_LOG(LogTemp, Display, TEXT("B64 intent: %s"),*intent);

	return intent;
}

FString ASequence_Backend_Manager::setup_random_wallet()
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

FString ASequence_Backend_Manager::get_main_url()
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

FString ASequence_Backend_Manager::get_continue_url()
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

FString ASequence_Backend_Manager::get_signin_url()
{
	FString signin_url;
	FString result;
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
		signin_url.Append(i);
	}

	FBase64::Decode(signin_url, result);
	return result;
}