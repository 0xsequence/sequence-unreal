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
	auto provider = Provider("http://localhost:8545/");
	return Get_Response(provider.BlockByNumber(1));
}
