#pragma once

#include "CoreMinimal.h"
#include "Async.h"
#include "JsonObjectConverter.h"
#include "Log.h"
#include "Containers/Union.h"
#include "Types/Types.h"
#include "Misc/Base64.h"
#include "Util/Structs/BE_Structs.h"
#include "Util/Structs/BE_Enums.h"
#include "SequenceSupport.generated.h"

struct FSeqGetTokenBalancesReturn;
struct FTokenBalanceExtractorReturn;
struct FSeqGetTransactionHistoryReturn;
struct FUpdatableItemDataArgs;

typedef TPair<FString, FString> FNameId;
typedef TPair<FString, FString> FIdName;

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FIdNamePair
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="0xSequence")
	FString NetworkId = "-1";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="0xSequence")
	FString NetworkName = "";

	FIdNamePair(){}

	FIdNamePair(const FIdName& IdNameIn)
	{
		NetworkId = IdNameIn.Key;
		NetworkName = IdNameIn.Value;
	}
};

/**
 * Support class for the indexer
 */
UCLASS()
class SEQUENCEPLUGIN_API USequenceSupport : public UObject
{
	GENERATED_BODY()

public:
	static void OpenExternalBrowser(const FString& Url);
	
	/*
	* Used to get the amount that users can read,
	* int64 Amount, the integer amount of Crypto
	* int64 Decimals, the integer amount of Decimal places used to represent the Amount
	* Return, The user readable Float amount
	*/
	static float GetUserReadableAmount(const int64 Amount, const int64 Decimals);
	
	/*
	* Used to convert a user readable amount into a usable amount value for transactions,
	* float Amount, the user readable amount they wish to use
	* int64 Decimals, the integer amount of Decimal places used to represent the Amount
	* Return, The transaction usable amount
	*/
	static int64 GetSystemReadableAmount(const float Amount, const int64 Decimals);

	static FString GetStringFromToken(const FString& IdToken, const FString& ParameterName);

	static int32 GetInt32FromToken(const FString& IdToken, const FString& ParameterName);
	
	static int64 GetInt64FromToken(const FString& IdToken, const FString& ParameterName);

	static float GetFloatFromToken(const FString& IdToken, const FString& ParameterName);

	static double GetDoubleFromToken(const FString& IdToken, const FString& ParameterName);

	static bool GetBoolFromToken(const FString& IdToken, const FString& ParameterName);
	
	static FString TransactionListToJsonString(const TArray<TransactionUnion>& Transactions);
	
	/*
	* Used to convert a jsonObject into a hash map of FStrings
	*/
	static TMap<FString, FString> JSONObjectParser(const TSharedPtr<FJsonObject>& JSONData);

	static TSharedPtr<FJsonObject> JsonStringToObject(const FString& JSON);
	
	/*
	* Used to remove
	* \n, \", \t, \r, spaces etc
	*/
	static FString SimplifyString(FString String);

	static FString PartialSimpleString(FString String);

	/*
	* Similar to simplify string EXCEPT we keep the /" because we are trying
	* to maintain json correctness for parsing!
	*/
	static FString SimplifyStringParsable(FString String);

	/*
	* Allows for the converting of a Struct straight into a nicely formatted json string!
	*/
	template < typename T > static FString StructToString(T StructVar)
	{
		FString Ret;
		FJsonObjectConverter::UStructToJsonObjectString<T>(StructVar, Ret, 0, 0);
		StringReplace(&Ret, "\n", "");
		StringReplace(&Ret, "\"None\"", "null");
		return Ret;
	}

	/*
	* Allows for the converting of a Struct straight into a nicely formatted json string!
	* This also cleans up the string from json formatting in the event we just want an
	* easily testable string!
	*/
	template < typename T > static FString StructToSimpleString(T StructVar)
	{
		FString Ret;
		FJsonObjectConverter::UStructToJsonObjectString<T>(StructVar, Ret, 0, 0);
		Ret = SimplifyString(Ret);
		return Ret;
	}

	/**
	 * Converts a UStruct into a Json usable String
	 * @tparam T Templated UStruct
	 * @param StructVar Struct to parse
	 * @return JsonString version of parsed UStruct
	 */
	template < typename T > static FString StructToPartialSimpleString(T StructVar)
	{
		FString Ret;
		FJsonObjectConverter::UStructToJsonObjectString<T>(StructVar, Ret, 0, 0);
		Ret = PartialSimpleString(Ret);
		return Ret;
	}

	template < typename T > static T JSONStringToStruct(FString JSON)
	{
		T Ret;

		TSharedPtr<FJsonObject> JsonObj = MakeShareable<FJsonObject>(new FJsonObject);

		if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JSON), JsonObj))
		{
			if (!FJsonObjectConverter::JsonObjectToUStruct<T>(JsonObj.ToSharedRef(), &Ret))
				SEQ_LOG(Error, TEXT("[Failed to convert jsonObject into a UStruct: [%s]]"), *JSON);
		}
		else
		{//failed to convert the decrypted string into a jsonObject
			SEQ_LOG(Error, TEXT("[Failed to convert jsonObjectString into a jsonObject: [%s]]"), *JSON);
		}

		return Ret;
	}

	template < typename T > static bool JSONStringToStruct(FString JSON, T * PtrRet)
	{
		T TStruct;
		bool Ret = false;
		TSharedPtr<FJsonObject> JsonObj = MakeShareable<FJsonObject>(new FJsonObject);

		if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JSON), JsonObj))
		{
			if (FJsonObjectConverter::JsonObjectToUStruct<T>(JsonObj.ToSharedRef(), &TStruct))
			{
				*PtrRet = TStruct;
				Ret = true;
			}
			else
			{
				SEQ_LOG(Error, TEXT("[Failed to convert jsonObject into a UStruct: [%s]]"), *JSON);
			}
		}
		else
		{//failed to convert the decrypted string into a jsonObject
			SEQ_LOG(Error, TEXT("[Failed to convert jsonObjectString into a jsonObject: [%s]]"), *JSON);
		}

		return Ret;
	}

	static bool ParseJsonStringToObjectArray(const FString& JsonString, TArray<TSharedPtr<FJsonValue>>& OutArray)
	{
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
		
		if (FJsonSerializer::Deserialize(Reader, OutArray) && OutArray.Num() > 0)
		{
			return true;
		}
		
		OutArray.Empty();
		return false;
	}

	static FString DecodeBase64ToString(const FString& Base64Input)
	{
		TArray<uint8> Bytes;
		if (FBase64::Decode(Base64Input, Bytes))
		{
			FString Out;
			FUTF8ToTCHAR Converter(reinterpret_cast<const ANSICHAR*>(Bytes.GetData()), Bytes.Num());
			Out = FString(Converter.Length(), Converter.Get());
			return Out;
		}

		return FString();
	}

	static FString JsonArrayToString(const TArray<TSharedPtr<FJsonValue>>& JsonValues)
	{
		FString OutputString;
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);

		FJsonSerializer::Serialize(JsonValues, Writer);
		return OutputString;
	}

	static TSharedPtr<FJsonValue> ParseJsonValue(const FString& JsonString)
	{
		TSharedPtr<FJsonValue> JsonValue;
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

		if (!FJsonSerializer::Deserialize(Reader, JsonValue))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON value: %s"), *JsonString);
			return nullptr;
		}

		return JsonValue;
	}

	static FString JsonValueToString(const TSharedPtr<FJsonValue>& JsonValue)
	{
		if (!JsonValue.IsValid())
		{
			return TEXT("null");
		}

		switch (JsonValue->Type)
		{
		case EJson::String:
			UE_LOG(LogTemp, Display, TEXT("JsonValue->Type is EJson::String"));
			return JsonValue->AsString();

		case EJson::Number:
			UE_LOG(LogTemp, Display, TEXT("JsonValue->Type is EJson::Number"));
			return FString::SanitizeFloat(JsonValue->AsNumber());

		case EJson::Boolean:
			UE_LOG(LogTemp, Display, TEXT("JsonValue->Type is EJson::Boolean"));
			return JsonValue->AsBool() ? TEXT("true") : TEXT("false");

		case EJson::Array:
			UE_LOG(LogTemp, Display, TEXT("JsonValue->Type is EJson::Array"));
			return USequenceSupport::JsonArrayToString(JsonValue->AsArray());

		case EJson::Object:
			UE_LOG(LogTemp, Display, TEXT("JsonValue->Type is EJson::Object"));
			return USequenceSupport::JsonToString(JsonValue->AsObject());

		default:
			return "";
		}
	}

	static FString StringListToSimpleString(TArray<FString> StringData);

	//for maintaining valid json for args in RPC calls
	static FString StringListToParsableString(TArray<FString> StringData);

	static TArray<TSharedPtr<FJsonValue>> StringListToJsonArray(const TArray<FString>& StringData);

	static FString Int64ListToSimpleString(TArray<int64> IntData);

	static FString JsonObjListToString(TArray<TSharedPtr<FJsonObject>> JsonData);
	
	static FString JsonObjListToSimpleString(TArray<TSharedPtr<FJsonObject>> JsonData);

	static FString JsonObjListToParsableString(TArray<TSharedPtr<FJsonObject>> JsonData);

	static FString JsonValueListToParsableString(const TArray<TSharedPtr<FJsonValue>>& JsonData);

	static FString JsonToString(const TSharedPtr<FJsonValue>& JsonData);

	static FString JsonToSimpleString(const TSharedPtr<FJsonValue>& JsonData);

	static FString JsonToParsableString(const TSharedPtr<FJsonValue>& JsonData);

	static FString JsonToParsableString(const TSharedPtr<FJsonObject>& JsonData);

	static FString JsonToString(const TSharedPtr<FJsonObject>& JsonData);

	static FString JsonToSimpleString(const TSharedPtr<FJsonObject>& JsonData);

	static int64 StringDateToUnixDate(const FString& Iso8601);

	/**
	 * Encode function parameters based on the function signature.
	 * @param FunctionSignature such as balanceOf(address,uint256)
	 * @param Values JSON array such as ["0x6615e4e985bf0d137196897dfa182dbd7127f54f", 2]
	 * @return Encoded hex string used to send as transaction data
	 */
	static FString EncodeFunctionCall(const FString& FunctionSignature, const FString& Values);

	/**
	 * The decoded data represents the ABI structure. In this case, it's an array of values in the array of 'outputs'
	 * Example: [[10000,0,"0x0000000000000000000000000000000000000000000000000000000000000000",1747809900,1905662700,"0x0000000000000000000000000000000000000000000000000000000000000000"]]
	 * @param Abi Function ABI in the following format: {"type": "function", ...}
	 * @param EncodedData The hex string you receive from a Call() function.
	 * @return Decoded data as an unnamed JSON array of values.
	 */
	static FString DecodeFunctionResult(const FString& Abi, const FString& EncodedData);

	/**
	 * Encode the given big integer into a byte array.
	 * @param Value The BigInteger value as a string type.
	 * @return The encoded byte array.
	 */
	static TArray<uint8> EncodeBigInteger(const FString& Value);

	static TArray<uint8> EncodeAndHashTypedData(const FString& DomainJson);

	static bool BigIntToBytes(const FString& ValueString, int32 Size, TArray<uint8>& OutBytes, FString& OutError);

	static bool RecoverEthPubAndAddress(const TArray<uint8>& Signature, const TArray<uint8>& AttestationHash, TArray<uint8>& OutPubKey, TArray<uint8>& OutAddress);

	static TArray<uint8> EncodeTwoAddresses(const FString& A, const FString& B);
	
private:
	/*
	* Similar to simplifyString EXCEPT we also cleanup 
	* some special edge cases from json responses / parsing as well!
	*/
	static FString StringCleanup(FString String);
	static void StringReplace(FString* Input, const FString& Search, const FString& Replacement);
};
