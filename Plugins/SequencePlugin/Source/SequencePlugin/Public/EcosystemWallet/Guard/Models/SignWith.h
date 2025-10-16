#pragma once

#include "CoreMinimal.h"
#include "SignWith.generated.h"

USTRUCT()
struct FGuardSignatureArgs
{
    GENERATED_BODY()

    UPROPERTY()
    FString Type;
    
    UPROPERTY()
    FString Address;

    UPROPERTY()
    FString ImageHash;

    UPROPERTY()
    FString Data;
};

USTRUCT(BlueprintType)
struct FSignRequest
{
    GENERATED_BODY()

    UPROPERTY()
    int64 ChainId;

    UPROPERTY()
    FString Msg;

    UPROPERTY()
    FString Wallet;

    UPROPERTY()
    FString PayloadType;

    UPROPERTY()
    FString PayloadData;

    UPROPERTY()
    TArray<FGuardSignatureArgs> Signatures;
};

USTRUCT()
struct FSignWithArgs
{
    GENERATED_BODY()

    UPROPERTY()
    FString Signer;

    UPROPERTY()
    FSignRequest Request;
};

USTRUCT()
struct FSignWithResponse
{
    GENERATED_BODY()

    UPROPERTY()
    FString Sig;
};
