// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "General_Testing.h"
#include "Tests/LargeTest.h"
#include "IndexerTests.h"
#include "JsonBuilder.h"
#include "RequestHandler.h"

// Sets default values
AGeneral_Testing::AGeneral_Testing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGeneral_Testing::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void AGeneral_Testing::test_provider()
{
	//stub in connect tests here!

	

	TFunction<void (FString)> OnSuccess = [this](FString State)
	{
		callback_passed(State);
	};

	TFunction<void (FString, SequenceError)> OnFailure = [this](FString data, SequenceError Err)
	{
		Callback_Failed(data, Err);
	};

	LargeTest(OnSuccess, OnFailure);
	
}

void AGeneral_Testing::test_indexer()
{
	//stub in test functions here!
	TFunction<void(FString)> OnSuccess = [this](FString State)
	{
		callback_passed(State);
	};

	TFunction<void(FString, SequenceError)> OnFailure = [this](FString data, SequenceError Err)
	{
		Callback_Failed(data, Err);
	};

	IndexerTest(OnSuccess, OnFailure);

}

void AGeneral_Testing::testMisc()
{
	FString testJson = "{\"nested_obj\":{\"pain\":101}, \"name\":\"test_object\",\"age\":1,\"list\":[\"one\",\"two\",\"three\"]}";
	TSharedPtr<FJsonObject> jsonObj = MakeShareable(new FJsonObject);
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(testJson);

	if (!FJsonSerializer::Deserialize(Reader, jsonObj))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to deserialize the json object!"));
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Built test JSON Object!"));
		TMap<FString, FString> tProp = UIndexerSupport::jsonObjectParser(jsonObj);
		TArray<FString> keys;
		tProp.GetKeys(keys);//get the keys
		UE_LOG(LogTemp, Display, TEXT("Resulting Property Map"));
		for (FString key : keys)
		{
			FString value = *tProp.Find(key);
			UE_LOG(LogTemp, Display, TEXT("Key: [%s] Value: [%s]"),*key, *value);
		}
	}
}

// Called every frame
void AGeneral_Testing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGeneral_Testing::callback_passed(FString state_data)
{
	UE_LOG(LogTemp, Display, TEXT("========================================================================="));
	UE_LOG(LogTemp, Display, TEXT("[Callback Passed!]\nAdditional State: [%s]"), *state_data);
	UE_LOG(LogTemp, Display, TEXT("========================================================================="));
}

void AGeneral_Testing::Callback_Failed(const FString state_data, SequenceError error) const
{
	UE_LOG(LogTemp, Display, TEXT("========================================================================="));
	UE_LOG(LogTemp, Error, TEXT("[Callback Failed!]\nAdditional State: [%s]"), *state_data);
	UE_LOG(LogTemp, Error, TEXT("[Error Message]:\n[%s]"),*error.Message);
	UE_LOG(LogTemp, Error, TEXT("[Error Type]: [%s]"),*Error_To_String(error.Type));
	UE_LOG(LogTemp, Display, TEXT("========================================================================="));
}

FString AGeneral_Testing::Error_To_String(ErrorType error)
{
	switch (error) {
	case NotFound:
		return "NotFound";
	case ResponseParseError:
		return "ResponseParseError";
	case EmptyResponse:
		return "EmptyResponse";
	case UnsupportedMethodOnChain:
		return "UnsupportedMethodOnChain";
	case RequestFail:
		return "RequestFail";
	case RequestTimeExceeded:
		return "RequestTimeExceeded";
	case TestFail:
		return "TestFail";
	default:
		return "SequenceError";
	}
}

