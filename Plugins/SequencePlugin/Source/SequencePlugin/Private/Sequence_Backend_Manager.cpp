// Fill out your copyright notice in the Description page of Project Settings.

#include "Sequence_Backend_Manager.h"
#include "Provider.h"
#include "HexUtility.h"
#include "Misc/AutomationTest.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Ecdsa.hpp"
#include "secp256k1Library/secp256k1/secp256k1.h"
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

template<typename T>
FString Get_Response(TResult<T> Result)
{

	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(Result.GetValue().ToSharedRef(), Writer);
	return OutputString;
}

FString ASequence_Backend_Manager::Setup()
{
	//Private Key 32 bytes long
	//ac80bd11fe77248b10b90ca3ba244667ca6f1c46aa137dedaf40d33dc5923cda
	//Public Key that we want for the above private key (64 bytes long)
	//4466d87ce078ee7e1760312c6fffade074794b4ed60ad4331185728a63354d97368955d50a0804b6d455ce049039ed825dc9ecde5087ee830266d59a088197b4
	//this is a simple test to generate a public key from a private key and see the results in editor
	FString result = "Test Failed";
	//Create a hashed 32byte private key (hard set for now)
	auto PrivateKey = HexStringToHash256("ac80bd11fe77248b10b90ca3ba244667ca6f1c46aa137dedaf40d33dc5923cda");
	FString hard_pub_key = "4466d87ce078ee7e1760312c6fffade074794b4ed60ad4331185728a63354d97368955d50a0804b6d455ce049039ed825dc9ecde5087ee830266d59a088197b4";
	PublicKey PublicKey = GetPublicKey(PrivateKey);
	
	//go from hash to hex!
	FString gen_pub_key = *HashToHexString(GPublicKeyByteLength,PublicKey);

	//check if they match
	if (gen_pub_key.Compare(hard_pub_key) == 0)
	{
		result = "Test Passed";//update test if passed
	}

	//log
	
	UE_LOG(LogTemp, Display, TEXT("Generated Public Key: %s"), (*HashToHexString(GPublicKeyByteLength, PublicKey)));
	UE_LOG(LogTemp, Display, TEXT("HardCoded Public Key: %s"), *hard_pub_key);
	return result;
}

FString ASequence_Backend_Manager::Testing_Address_Generation()
{
	FString Result = "Test Failed";
	//Note we may need to cutoff the first byte (0x if it's present as it would result in 33 bytes not 32)
	FString accnt_0_prvt_key = "abc0000000000000000000000000000000000000000000000000000000000002";
	FString accnt_0_pblc_key_hard = "2ad49951b009ddfb436cf7f6ab485ef5bb4ad1f19c41f28d447a9c63866a69b11e0de95f630b5bfc8c77d357e81e5bab8aff4609a89b7420c2f776d6a030197a";
	FString accnt_0_addr_hard = "c683a014955b75F5ECF991d4502427c8fa1Aa249";

	//go from hash to hex for testing!
	FString accnt_0_pblc_key_Vis = *HashToHexString(GPublicKeyByteLength,GetPublicKey(HexStringToHash256(accnt_0_prvt_key)));
	//go straight to hash for usage!
	Hash accnt_0_pblc_key = GetPublicKey(HexStringToHash256(accnt_0_prvt_key));
	//generate account address based on public key!
	Hash accnt_0_addr = GetAddress(accnt_0_pblc_key);


	UE_LOG(LogTemp, Display, TEXT("Private Key: %s"), *accnt_0_prvt_key);
	UE_LOG(LogTemp, Display, TEXT("Public Gen Key: %s"), *accnt_0_pblc_key_Vis);
	UE_LOG(LogTemp, Display, TEXT("Public HRD Key: %s"), *accnt_0_pblc_key_hard);
	UE_LOG(LogTemp, Display, TEXT("Gen Address: %s"), *HashToHexString(GPublicKeyByteLength,accnt_0_addr));
	UE_LOG(LogTemp, Display, TEXT("Hrd Address: %s"), *accnt_0_addr_hard);

	FString accnt_0_addr_hx = *HashToHexString(GPublicKeyByteLength, accnt_0_addr);

	if (accnt_0_addr_hard.Compare(accnt_0_addr_hx) == 0 && accnt_0_pblc_key_Vis.Compare(accnt_0_pblc_key_hard) == 0)
	{
		Result = "Test Passed";
	}

	return Result;
}