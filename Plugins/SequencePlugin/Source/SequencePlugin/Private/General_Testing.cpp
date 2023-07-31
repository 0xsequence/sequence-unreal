// Fill out your copyright notice in the Description page of Project Settings.


#include "General_Testing.h"

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
}

void AGeneral_Testing::test_indexer()
{
	//stub in test functions here!
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

void AGeneral_Testing::callback_failed(FString state_data, SequenceError error)
{
	UE_LOG(LogTemp, Display, TEXT("========================================================================="));
	UE_LOG(LogTemp, Error, TEXT("[Callback Failed!]\nAdditional State: [%s]"), *state_data);
	UE_LOG(LogTemp, Error, TEXT("[Error Message]:\n[%s]"),*error.Message);
	UE_LOG(LogTemp, Error, TEXT("[Error Type]: [%s]"),*error_to_string(error.Type));
	UE_LOG(LogTemp, Display, TEXT("========================================================================="));
}

FString AGeneral_Testing::error_to_string(ErrorType error)
{
	FString ret = "";
	switch (error) {
	case 0:
		ret = "NotFound";
		break;
	case 1:
		ret = "ResponseParseError";
		break;
	case 2:
		ret = "EmptyResponse";
		break;
	case 3:
		ret = "UnsupportedMethodOnChain";
		break;
	case 4:
		ret = "RequestFail";
		break;
	}
	return ret;
}

