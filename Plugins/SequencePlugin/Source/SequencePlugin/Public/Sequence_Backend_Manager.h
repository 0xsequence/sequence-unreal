// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sequence_Backend_Manager.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API ASequence_Backend_Manager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASequence_Backend_Manager();

	/*
	* Used to setup the backend manager for usage
	* retuns FString ("Complete Message")
	*/
	UFUNCTION(BlueprintCallable)
		FString Setup();

	/*
	* Used to test an indivual private key, to see how it's generated public key compares to a CORRECT
	* pre computed and and how the generated address from the generated public key compares to a precomputed
	* CORRECT address
	* returns true IFF gen_public_key matches the hard_public_key &&
	* gen_address matches hard_address
	*/
	bool test_address_gen(FString prvt_k, FString hrd_pblc_k, FString hrd_addr);

	/*
	* Used to test the public key generation and address generation in accordance with
	* the test chain!
	*/
	UFUNCTION(BlueprintCallable)
		FString Testing_Address_Generation();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
