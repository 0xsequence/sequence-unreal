// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "SeqTokenList.generated.h"

/*
    These structs exist because UE T structures don't support nesting BUT do allow us to wrap T structures
    in UStructs allowing us to support nesting
*/

// For use in GetTokenSuppliesMapArgs
USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTokenList
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
    TArray<FString> token_list;

    // Default constructor
    FSeqTokenList() {}

    // Constructor to initialize with a variadic list of FString arguments
    template<typename... Args>
    FSeqTokenList(Args... args)
    {
        AddTokens(args...);
    }

private:
    // Helper function to unpack variadic arguments and add them to the token_list
    template<typename... Args>
    void AddTokens(Args... args)
    {
        token_list = { args... };
    }
};
