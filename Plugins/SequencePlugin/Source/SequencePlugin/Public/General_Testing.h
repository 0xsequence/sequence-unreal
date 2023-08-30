// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Errors.h"
#include "Indexer/IndexerSupport.h"
#include "SequenceDataTests.h"
#include "General_Testing.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API AGeneral_Testing : public AActor
{
	GENERATED_BODY()

private:
	TArray<FString> testingURLs;
	UObjectHandler* imgHandler;
public:	
	// Sets default values for this actor's properties
	AGeneral_Testing();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

//Testing Functions//

	/*
	* Called by frontend to test the provider!
	*/
	UFUNCTION(BlueprintCallable, CATEGORY = "Test_Provider")
		void TestProvider() const;

	/*
	* Called by frontend to test the indexer
	*/
	UFUNCTION(BlueprintCallable, CATEGORY = "Test_Indexer")
		void TestIndexer();

	UFUNCTION(BlueprintCallable, CATEGORY = "Test_Encryption")
		void TestEncryption() const;

	/*
	* Used to test misc functions / support objects!
	*/
	UFUNCTION(BlueprintCallable, CATEGORY = "Test_Misc")
		void TestMisc();

	//special bindable function used for handling callbacks on Async stuff!
	UFUNCTION()
		void OnDoneImageProcessing();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, CATEGORY = "Test_Misc")
		void testMiscForwarder(const TMap<FString, UTexture2D*>& imageMap);

	UFUNCTION(BlueprintCallable, CATEGORY = "Test_Sequence_Layer")
		void TestSequence() const;

	UFUNCTION(BlueprintCallable, CATEGORY = "Test_SystemDataBuiler")
		void testSystemDataBuilder();
//End of Testing Functions//
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//These functions are for handling the callback pass and callback fail cases in testing!

	/*
	* Here I include some additional state_data about the callback that can be printed
	* in with the on successcallback!
	*/
	void CallbackPassed(FString StateData) const;

	/*
	* Here I include some additional state data about the callback as well as
	* error data that get printed out in the callback failure!
	*/
	void CallbackFailed(FString StateData, FSequenceError Error) const;

	/*
	* Used to convert an enum to a readable string
	*/
	static FString ErrorToString(EErrorType Error);

};
