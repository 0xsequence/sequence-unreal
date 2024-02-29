

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SignedMessage.generated.h"
//{"response":{"code":"signedMessage","data":{"message":"","signature":""}}}

USTRUCT()
struct FSignedMessageSignature
{
	GENERATED_BODY()
	UPROPERTY()
	FString message = "";
	UPROPERTY()
	FString signature = "";
};

USTRUCT()
struct FSignedMessage
{
	GENERATED_BODY()

	UPROPERTY()
	FSignedMessageSignature data;
};

USTRUCT()
struct FSignedMessageResponseObj
{
	GENERATED_BODY()

	UPROPERTY()
	FSignedMessage response;
};