// Fill out your copyright notice in the Description page of Project Settings.

#include "Sequence_Backend_Manager.h"
#include "Provider.h"
#include "HexUtility.h"
#include "Misc/AutomationTest.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Ecdsa.hpp"
#include "Crypto.h"

// Sets default values
ASequence_Backend_Manager::ASequence_Backend_Manager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

void ASequence_Backend_Manager::Testing_Indexer()
{
	UIndexer *indexer = NewObject<UIndexer>();//Create an object using templates!!!!
	UE_LOG(LogTemp, Display, TEXT("Indexer tests starting\n"));
	indexer->testing();
	UE_LOG(LogTemp, Display, TEXT("Indexer tests Done"));
}
