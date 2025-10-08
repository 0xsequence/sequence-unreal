#pragma once

#include "CoreMinimal.h"
#include "SignWith.generated.h"

UENUM()
enum class EGuardPayloadType : uint8
{
    Calls                UMETA(DisplayName = "Calls"),
    Message              UMETA(DisplayName = "Message"),
    ConfigUpdate         UMETA(DisplayName = "ConfigUpdate"),
    SessionImplicitAuthorize UMETA(DisplayName = "SessionImplicitAuthorize")
};

UENUM()
enum class EGuardSignatureType : uint8
{
    Hash    UMETA(DisplayName = "Hash"),
    Sapient UMETA(DisplayName = "Sapient"),
    EthSign UMETA(DisplayName = "EthSign"),
    Erc1271 UMETA(DisplayName = "Erc1271")
};

USTRUCT()
struct FGuardSignatureArgs
{
    GENERATED_BODY()

    UPROPERTY()
    FString Address;

    UPROPERTY()
    EGuardSignatureType Type;

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
    EGuardPayloadType PayloadType;

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
