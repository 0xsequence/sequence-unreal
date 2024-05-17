// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Errors.h"
#include "ObjectHandler.h"
#include "GeneralTesting.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API AGeneralTesting : public AActor
{
	GENERATED_BODY()

private:
	TArray<FString> testingURLs = {"https://assets.coingecko.com/coins/images/6319/large/USD_Coin_icon.png?1547042389","https://www.circle.com/hubfs/share-USDC.png#keepProtocol","https://assets.skyweaver.net/i7FuksL3/webapp/cards/full-cards/4x/0-silver.png","https://skyweaver.net/images/skyweavercover.jpg"};

	UPROPERTY()
	UObjectHandler * imgHandler;
public:	
	// Sets default values for this actor's properties
	AGeneralTesting();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

//Testing Functions//
	UFUNCTION(BlueprintCallable, CATEGORY = "Test_API")
	void TestRegisterSession() const;

	UFUNCTION(BlueprintCallable, CATEGORY = "Test_API")
	void TestSignMessage() const;
	
	UFUNCTION(BlueprintCallable, CATEGORY = "Test_API")
	void TestListSessions() const;
	
	UFUNCTION(BlueprintCallable, CATEGORY = "Test_API")
	void TestCloseSessions() const;

	UFUNCTION(BlueprintCallable, CATEGORY = "Test_API")
	void TestTokenBalances() const;

	UFUNCTION(BlueprintCallable, CATEGORY = "Test_API")
	void TestHistory() const;

	UFUNCTION(BlueprintCallable, CATEGORY = "Test_API")
	void TestSendRaw() const;

	UFUNCTION(BlueprintCallable, CATEGORY = "Test_API")
	void TestCallContract() const;

	UFUNCTION(BlueprintCallable, CATEGORY = "Test_API")
	void TestSendERC20() const;

	UFUNCTION(BlueprintCallable, CATEGORY = "Test_API")
	void TestSendERC721() const;

	UFUNCTION(BlueprintCallable, CATEGORY = "Test_API")
	void TestSendERC1155() const;
	
	/*
	* Called by frontend to test the indexer
	*/
	UFUNCTION(BlueprintCallable, CATEGORY = "Test_Indexer")
		void TestIndexer();

	/*
	* Called by frontend to test the Encryptor
	*/
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

	/*
	* Used for forwarding data to the Frontend for displaying the images we received in testing
	*/
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, CATEGORY = "Test_Misc")
		void testMiscForwarder(const TMap<FString, UTexture2D*>& imageMap);

	//These 2 functions were used to test functionality that is no longer in the current version
	//UFUNCTION(BlueprintCallable, CATEGORY = "Test_Sequence_Layer")
	//	void TestSequence() const;

	//UFUNCTION(BlueprintCallable, CATEGORY = "Test_SystemDataBuiler")
	//	void testSystemDataBuilder();

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