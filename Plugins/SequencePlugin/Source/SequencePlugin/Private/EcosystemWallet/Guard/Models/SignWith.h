#pragma once

#include "CoreMinimal.h"
#include "SignWith.generated.h"

USTRUCT()
struct FGuardSignatureArgs
{
    GENERATED_BODY()

    UPROPERTY()
    FString Address;

    UPROPERTY()
    FString Type;

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
    FString ChainId;

    UPROPERTY()
    FString Msg;

    UPROPERTY()
    FString AuxData;

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
