#pragma once

#include "CoreMinimal.h"
#include "BE_Enums.h"
#include "BE_Structs.generated.h"

USTRUCT(BlueprintType)
struct FNetworkProfileSetting_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Indexer_URL;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Relayer_URL;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Block_Explorer_URL;
};

USTRUCT(BlueprintType)
struct FNetwork_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Network_Name;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* Network_Symbol;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FNetworkProfileSetting_BE Network_Settings;
};

USTRUCT(BlueprintType)
struct FSystemSession_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString System_Session;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString System_Session_Key;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* System_Session_Icon;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FNetwork_BE Network;
};

USTRUCT(BlueprintType)
struct FActiveSession_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Current_Session;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Session_Key;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Last_Seen_Date_Time;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Platform_Data;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* Session_Icon;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FNetwork_BE Network;
};

USTRUCT(BlueprintType)
struct FContact_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Nickname;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Public_Address;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* Profile_Icon;
};

USTRUCT(BlueprintType)
struct FFee_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Fee_Long_Name;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Fee_Short_Name;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Formatted_Fee_Amount;//parser exists in frontend
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Formatted_Fee_Value;//parser exists in frontend
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* Fee_Icon;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        float Fee_Amount;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        float Fee_Value;
};

USTRUCT(BlueprintType)
struct FFiatCurrency_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Long_Name;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Short_Name;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Symbol;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        float Mult;//NOP value for right now until I know how currency data is structured
};

USTRUCT(BlueprintType)
struct FSelectableCurrency_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Long_Name;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Short_Name;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Symbol;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        float Mult;//NOP value for right now until I know how currency data is structured
};

USTRUCT(BlueprintType)
struct FHistoryElement_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Item_Name;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Item_Short_Name;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Token_Collection_Name;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Collection_Short_Name;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* Collection_Icon;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        float Item_Amount;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        float Value_Per_Item;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* Item_Icon;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TEnumAsByte<EHistoryItemType> History_Item_Type;
};

USTRUCT(BlueprintType)
struct FMonthDayYear_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        int32 Month;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        int32 Day;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        int32 Year;
};

USTRUCT(BlueprintType)
struct FTokenHistoryCollection_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Collection_Name;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Collection_Short_Name;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TArray<FHistoryElement_BE> Token_Data;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* Collection_Icon;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        bool From_To;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TEnumAsByte<ETXNType> TXN_Type;//this enum consists of send,receive,swap the base txnTransfer type only has send and receive!
};

USTRUCT(BlueprintType)
struct FCoin_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* Coin_Symbol;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Coin_Long_Name;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Coin_Short_Name;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        float Coin_Amount;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        float Coin_Value;//per 1 coin!
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Formatted_Amount;//parser in frontend
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Formatted_Total_Value;//perser in frontend
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Formatted_Single_Value;//parser in frontend
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Coin_Standard;//ERC 1155, etc
};


USTRUCT(BlueprintType)
struct FNFTDetails_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString External_URL;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Contract_Address;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        int32 Token_ID;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Token_Standard;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Network;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* Network_Icon;
};

USTRUCT(BlueprintType)
struct FNFT_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* NFT_Icon;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString NFT_Name;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString NFT_Short_Name;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Collection_Long_Name;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Collection_Short_Name;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* Collection_Icon;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        int32 Amount;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        float Value;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Formatted_Total_Value;//Parser in frontend
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Formatted_Single_Value;//Parser in frontend
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Description;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TMap<FString, FString> Properties;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FNFTDetails_BE NFT_Details;

};