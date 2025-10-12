#pragma once

#include "CoreMinimal.h"
#include "SessionArgs.h"
#include "ConnectArgs.generated.h"

USTRUCT()
struct FConnectArgs
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString PreferredLoginMethod;

	UPROPERTY()
	FString Email;

	UPROPERTY()
	FString Origin;
	
	UPROPERTY()
	bool IncludeImplicitSession = false;

	UPROPERTY()
	FSessionArgs Session;

	bool CustomJson = true;

	FString ToJson() const
	{
		TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();

		JsonObject->SetStringField(TEXT("preferredLoginMethod"), PreferredLoginMethod);
		JsonObject->SetStringField(TEXT("email"), Email);
		JsonObject->SetStringField(TEXT("origin"), Origin);
		JsonObject->SetBoolField(TEXT("includeImplicitSession"), IncludeImplicitSession);

		TSharedPtr<FJsonObject> SessionObject = Session.ToJson();
		JsonObject->SetObjectField(TEXT("session"), SessionObject);

		FString OutputString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(JsonObject, Writer);

		UE_LOG(LogTemp, Display, TEXT("Connect Args: %s"), *OutputString);

		return OutputString;
	}
};
