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

	this->indexer = NewObject<UIndexer>();
	this->indexer->setup(this);//pass the indexer a ref. to ourselves so it can let us know when stuff is done!
}

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

/*
* Initiates the passwordless signin process
*/
FString ASequence_Backend_Manager::Signin(FString email)
{
	this->user_email = email;
	post_json_request(get_main_url(), create_blk_req(), &ASequence_Backend_Manager::get_blk_handler);

	return "";
}

void ASequence_Backend_Manager::get_ether_balance()
{
	this->indexer->GetEtherBalance(glb_ChainID,glb_PublicAddress);
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
/*
* Used to setup the backend manager for usage
* retuns FString ("Complete Message")
*/
FString ASequence_Backend_Manager::Setup()
{
	return "Setup Complete";
}

/*
* Used to test an indivual private key, to see how it's generated public key compares to a CORRECT
* pre computed and and how the generated address from the generated public key compares to a precomputed
* CORRECT address
* returns true IFF gen_public_key matches the hard_public_key &&
* gen_address matches hard_address
*/
bool ASequence_Backend_Manager::test_address_gen(FString prvt_k, FString hrd_pblc_k, FString hrd_addr)
{
	bool bResult = false;

	//gen the public key one that's usable and the other for printing / testing!
	auto PrivKey = FPrivateKey::From(prvt_k);
	FPublicKey gen_pblc_key_H = GetPublicKey(PrivKey);
	FString gen_pblc_k_V = gen_pblc_key_H.ToHex();

	FAddress addr_H = GetAddress(gen_pblc_key_H);
	FString addr_V = addr_H.ToHex();
	UE_LOG(LogTemp, Display, TEXT("=========================================================================="));
	UE_LOG(LogTemp, Display, TEXT("Private Key: %s"), *prvt_k);
	UE_LOG(LogTemp, Display, TEXT("Public Gen Key: %s"), *gen_pblc_k_V);
	UE_LOG(LogTemp, Display, TEXT("Public HRD Key: %s"), *hrd_pblc_k);
	UE_LOG(LogTemp, Display, TEXT("Gen Address: %s"), *addr_V);
	UE_LOG(LogTemp, Display, TEXT("Hrd Address: %s"), *hrd_addr);

	hrd_addr = hrd_addr.ToLower();//because this all needs to be lower case to work!
	hrd_pblc_k = hrd_pblc_k.ToLower();
	addr_V = addr_V.ToLower();
	gen_pblc_k_V = gen_pblc_k_V.ToLower();
	if (hrd_addr.Compare(addr_V) == 0 && gen_pblc_k_V.Compare(hrd_pblc_k) == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Test Passed"));
		bResult = true;
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Test Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("=========================================================================="));
	return bResult;
}

/*
* Used to test the public key generation and address generation in accordance with
* the test chain!
*/
FString ASequence_Backend_Manager::Testing_Address_Generation()
{
	FString Result = "Test Failed";
	bool bResult = true;
	//account 1
	FString a0_prvt_k = "abc0000000000000000000000000000000000000000000000000000000000001";
	FString a0_pblc_k_h = "ef345e93b9c10b43b42ae5e1322cba136cf1553c8e9ae065b89d8dcea02badaf22561f1b8bb92f60c5c003a7898c5f785cef6496819c862a18c50b22f7b4d17f";
	FString a0_addr_h = "c683a014955b75f5ecf991d4502427c8fa1aa249";
	bResult &= test_address_gen(a0_prvt_k, a0_pblc_k_h, a0_addr_h);

	//account 2
	FString a1_prvt_k = "abc0000000000000000000000000000000000000000000000000000000000002";
	FString a1_pblc_k_h = "2ad49951b009ddfb436cf7f6ab485ef5bb4ad1f19c41f28d447a9c63866a69b11e0de95f630b5bfc8c77d357e81e5bab8aff4609a89b7420c2f776d6a030197a";
	FString a1_addr_h = "1099542d7dfaf6757527146c0ab9e70a967f71c0";
	bResult &= test_address_gen(a1_prvt_k, a1_pblc_k_h, a1_addr_h);

	//account 3
	FString a2_prvt_k = "abc0000000000000000000000000000000000000000000000000000000000003";
	FString a2_pblc_k_h = "515d9e6b30d7602acf4c58129ef87e893109175a2a9653ce12c50d56f5119b7a9ea16ce02f796e272f82318e141fb786752dbe667787667f02f109a953dfc3a7";
	FString a2_addr_h = "606e6d28e9150D8A3C070AEfB751a2D0C5DB19fa";
	bResult &= test_address_gen(a2_prvt_k, a2_pblc_k_h, a2_addr_h);
	
	//account 4
	FString a3_prvt_k = "abc0000000000000000000000000000000000000000000000000000000000004";
	FString a3_pblc_k_h = "c96393b446ef90ab2e2cd3d216f14efc475ad0b64992671d5f435a9f6db0768f0f68d673742aef4a894233e40f6f49f3ef512b89dd8131343bcbc6ca032529aa";
	FString a3_addr_h = "b396CbD9b745Ffc4a9C9A6D43D7957b1350Be153";
	bResult &= test_address_gen(a3_prvt_k, a3_pblc_k_h, a3_addr_h);

	//account 5
	FString a4_prvt_k = "abc0000000000000000000000000000000000000000000000000000000000005";
	FString a4_pblc_k_h = "1b42044486d6d22cbd1ea5e113add43011bdea1979e44f1df1b6e2d8e4ebc2e0f031d773ffb1e4c65b78d6ce906b352288fc1b51686cfb9925402b979a904807";
	FString a4_addr_h = "6F5Ddb00e3cb99Dfd9A07885Ea91303629D1DA94";
	bResult &= test_address_gen(a4_prvt_k, a4_pblc_k_h, a4_addr_h);

	//account 6
	FString a5_prvt_k = "abc0000000000000000000000000000000000000000000000000000000000006";
	FString a5_pblc_k_h = "1d6069a50f0df1488654757302dc0cfbbccced0dc740f205c8871d0700b20753bb703ac8b47ac7e86f65842401b25321446a90c28167fd4d9bab2ab38c62b3f6";
	FString a5_addr_h = "3F96a0D6697e5E7ACEC56A21681195dC6262b06C";
	bResult &= test_address_gen(a5_prvt_k, a5_pblc_k_h, a5_addr_h);

	if (bResult)
	{
		Result = "Test Passed";
	}
	return Result;
}

TArray<UTexture2D*> ASequence_Backend_Manager::Testing_Indexer()
{
	TArray<UTexture2D*> ret = this->indexer->testing();
	return ret;
}


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
	FString *ret;
	FString ret_data;
	ret = &ret_data;
	FPlatformMisc::ClipboardPaste(*ret);//get data from the clip board!
	//again platform misc is deprecated :( but until the generic platform clipboard functions are
	//actually implemented this is the best I can do for the time being.
	return ret_data;
}
