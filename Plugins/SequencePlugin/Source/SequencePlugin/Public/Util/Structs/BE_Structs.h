
#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "Dom/JsonValue.h"
#include "Sequence/DelayedEncodingBP.h"
#include "BE_Structs.generated.h"

//Sequence API Structs

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FDelayedTransaction
{
    GENERATED_USTRUCT_BODY()
private:
    FString type = "delayedEncode";
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    UDelayedEncodingBP * data = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    FString to = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    FString value = "";

    FDelayedTransaction()
    {
        data = NewObject<UDelayedEncodingBP>();
    }
    
    FString GetJsonString() const
    {
        FString Json = "{";
        Json += "\"data\":" + data->GetJsonString() + ",";
        Json += "\"to\":\"" + to + "\",";
        Json += "\"type\":\"" + type + "\",";
        Json += "\"value\":\"" + value + "\"";
        Json += "}";
        return Json;
    }
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FRawTransaction
{
    GENERATED_USTRUCT_BODY()
private:
    UPROPERTY()
    FString type = "transaction";
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    FString to = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    FString data = "null";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    FString value = "";

    FRawTransaction(){}
		
    FRawTransaction(const FString& ToIn, const FString& DataIn, const int32& ValueIn)
    {
        to = ToIn;
        data = DataIn;
        value = FString::Printf(TEXT("%d"),ValueIn);
    }

    FRawTransaction(const FString& ToIn, const FString& DataIn, const FString& ValueIn)
    {
        to = ToIn;
        data = DataIn;
        value = ValueIn;
    }

    FString GetType() const
    {
        return type;
    }

    FString GetJsonString() const
    {
        FString Json = "{";
        Json += "\"data\":\"" + data + "\",";
        Json += "\"to\":\"" + to + "\",";
        Json += "\"type\":\"" + type + "\",";
        Json += "\"value\":\"" + value + "\"";
        Json += "}";
        return Json;
    }
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FERC20Transaction
{
	GENERATED_USTRUCT_BODY()
private:
	UPROPERTY()
	FString type = "erc20send";
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString to = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString value = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString tokenAddress = "";

	FERC20Transaction(){}
	FERC20Transaction(const FString& ToIn, const int32& ValueIn, const FString& TokenAddressIn)
	{
		to = ToIn;
		value = FString::Printf(TEXT("%d"),ValueIn);
		tokenAddress = TokenAddressIn;
	}
	FERC20Transaction(const FString& ToIn, const FString& ValueIn, const FString& TokenAddressIn)
	{
		to = ToIn;
		value = ValueIn;
		tokenAddress = TokenAddressIn;
	}

	FString GetType() const
	{
		return type;
	}

    FString GetJsonString() const
	{
	    FString Json = "{";
        Json += "\"to\":\"" + to + "\",";
	    Json += "\"tokenAddress\":\"" + tokenAddress + "\",";
	    Json += "\"type\":\"" + type + "\",";
	    Json += "\"value\":\"" + value + "\"";
	    Json += "}";
	    return Json;
	}
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FERC721Transaction
{
	GENERATED_USTRUCT_BODY()
private:
	UPROPERTY()
	FString type = "erc721send";
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString to = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    FString id = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString data = "null";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString tokenAddress = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	bool safe = true;

	FERC721Transaction(){}
		
	FERC721Transaction(const FString& ToIn, const FString& IdIn, const FString& DataIn, const FString& TokenAddressIn, const bool& SafeIn)
	{
		to = ToIn;
	    id = IdIn;
		data = DataIn;
		tokenAddress = TokenAddressIn;
		safe = SafeIn;
	}

	FString GetType() const
	{
		return type;
	}

    FString GetJsonString() const
	{
	    const FString safeState = (safe) ? "true" : "false";
	    FString Json = "{";
	    Json += "\"data\":\"" + data + "\",";
	    Json += "\"id\":\"" + id + "\",";
	    Json += "\"safe\":" + safeState + ",";
	    Json += "\"to\":\"" + to + "\",";
	    Json += "\"tokenAddress\":\"" + tokenAddress + "\",";
	    Json += "\"type\":\"" + type + "\"";
	    Json += "}";
	    return Json;
	}
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FERC1155TxnValue
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString amount = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString id = "";

	FERC1155TxnValue(){}
		
	FERC1155TxnValue(const int32& AmountIn, const FString& IdIn)
	{
		amount = FString::Printf(TEXT("%d"),AmountIn);
		id = IdIn;
	}

	FERC1155TxnValue(const FString& AmountIn, const FString& IdIn)
	{
		amount = AmountIn;
		id = IdIn;
	}

    FString GetJsonString() const
	{
	    FString Json = "{";
	    Json += "\"amount\":\"" + amount + "\",";
	    Json += "\"id\":\"" + id + "\"";
	    Json += "}";
	    return Json;
	}
};

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FERC1155Transaction
{
	GENERATED_USTRUCT_BODY()
private:
	UPROPERTY()
	FString type = "erc1155send";
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString to = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString data = "null";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString tokenAddress = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	TArray<FERC1155TxnValue> vals;
		
	FERC1155Transaction(){}
		
	FERC1155Transaction(const FString& ToIn, const FString& DataIn, const FString& TokenAddressIn, TArray<FERC1155TxnValue> ValsIn)
	{
		to = ToIn;
		data = DataIn;
		tokenAddress = TokenAddressIn;
		vals.Reset(ValsIn.Num());
		vals.Append(ValsIn);
	}

	FString GetType() const
	{
		return type;
	}

    FString GetJsonString() const
	{
	    FString Json = "{";
	    Json += "\"data\":\"" + data + "\",";
	    Json += "\"to\":\"" + to + "\",";
	    Json += "\"tokenAddress\":\"" + tokenAddress + "\",";
	    Json += "\"type\":\"" + type + "\",";
	    Json += "\"vals\":";
	    Json += "[";
	    for (auto val : vals)
	    {
	        Json += val.GetJsonString() + ",";
	    }
	    Json.RemoveAt(Json.Len()-1);//remove the last trailing ,
	    Json += "]";
	    Json += "}";
	    return Json;
	}
};
