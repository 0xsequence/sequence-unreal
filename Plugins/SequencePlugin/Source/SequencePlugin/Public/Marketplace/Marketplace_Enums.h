#pragma once

#include "CoreMinimal.h"
#include "Marketplace_Enums.generated.h"

UENUM(BlueprintType)
enum EMarkeplaceSortOrder
{
    DESCENDING UMETA(DisplayName = "Descending"),
    ASCENDING  UMETA(DisplayName = "Ascending")
};

// PropertyType Enum
UENUM(BlueprintType)
enum EMarketplacePropertyType
{
    INT_M     UMETA(DisplayName = "Integer"),
    STRING_M  UMETA(DisplayName = "String"),
    ARRAY_M   UMETA(DisplayName = "Array"),
    GENERIC_M UMETA(DisplayName = "Generic")
};


// MarketplaceKind Enum
UENUM(BlueprintType)
enum EMarketplaceKind
{
    UNKNOWN_MK              UMETA(DisplayName = "Unknown"),
    SEQUENCE_MARKETPLACE_V1 UMETA(DisplayName = "Sequence Marketplace V1"),
    SEQUENCE_MARKETPLACE_V2 UMETA(DisplayName = "Sequence Marketplace V2"),
    OPENSEA                 UMETA(DisplayName = "Opensea"),
    MAGIC_EDEN              UMETA(DisplayName = "Magic Eden"),
    MINTIFY                 UMETA(DisplayName = "Mintify"),
    LOOKS_RARE              UMETA(DisplayName = "Looks Rare"),
    X2Y2                    UMETA(DisplayName = "X2Y2"),
    SUDO_SWAP               UMETA(DisplayName = "Sudo Swap"),
    COINBASE                UMETA(DisplayName = "Coinbase"),
    RARIBLE                 UMETA(DisplayName = "Rarible"),
    NFTX                    UMETA(DisplayName = "NFTX"),
    FOUNDATION              UMETA(DisplayName = "Foundation"),
    MANIFOLD                UMETA(DisplayName = "Manifold"),
    ZORA                    UMETA(DisplayName = "Zora"),
    BLUR                    UMETA(DisplayName = "Blur"),
    SUPER_RARE              UMETA(DisplayName = "Super Rare"),
    OKX                     UMETA(DisplayName = "OKX"),
    ELEMENT                 UMETA(DisplayName = "Element"),
    AQUA_XYZ                UMETA(DisplayName = "Aqua XYZ"),
    AURANFT_CO              UMETA(DisplayName = "Auranft CO")
};

UENUM(BlueprintType)
enum EOrderSide
{
    UNKNOWN_OSD  UMETA(DisplayName = "Unknown"),
    LISTING UMETA(DisplayName = "Listing"),
    OFFER UMETA(DisplayName = "Offer"),
};

UENUM(BlueprintType)
enum EOrderStatus
{
    UNKNOWN_OST UMETA(DisplayName = "Unknown"),
    ACTIVE  UMETA(DisplayName = "Active"),
    INACTIVE  UMETA(DisplayName = "Inactive"),
    EXPIRED  UMETA(DisplayName = "Expired"),
    CANCELLED  UMETA(DisplayName = "Cancelled"),
    FILLED  UMETA(DisplayName = "Filled"),
};

UENUM(BlueprintType)
enum class EOrderbookKind : uint8
{
    Unknown UMETA(DisplayName = "unknown"),
    Sequence_Marketplace_V1 UMETA(DisplayName = "sequence_marketplace_v1"),
    Sequence_Marketplace_V2 UMETA(DisplayName = "sequence_marketplace_v2"),
    Blur UMETA(DisplayName = "blur"),
    Opensea UMETA(DisplayName = "opensea"),
    Looks_Rare UMETA(DisplayName = "looks_rare"),
    Reservoir UMETA(DisplayName = "reservoir"),
    X2Y2 UMETA(DisplayName = "x2y2")
};


// PropertyType Extensions
UCLASS()
class UMarketplaceEnumsExtensions : public UObject
{
    GENERATED_BODY()

public:

    template<typename EnumType>
    static FString AsString(EnumType EnumValue);

    template<>
    FString AsString<EOrderbookKind>(const EOrderbookKind OrderbookKind)
    {
        switch (OrderbookKind)
        {
            case EOrderbookKind::Sequence_Marketplace_V1:
                return FString("sequence_marketplace_v1");
            case EOrderbookKind::Sequence_Marketplace_V2:
                return FString("sequence_marketplace_v2");
            case EOrderbookKind::Blur:
                return FString("blur");
            case EOrderbookKind::Opensea:
                return FString("opensea");
            case EOrderbookKind::Looks_Rare:
                return FString("looks_rare");
            case EOrderbookKind::Reservoir:
                return FString("reservoir");
            case EOrderbookKind::X2Y2:
                return FString("x2y2");
            default: return FString("unknown");
        }
    }
    
    template<typename EnumType>
    static FString AsString(TEnumAsByte<EnumType> EnumValue);

    template<>
    FString AsString<EMarketplacePropertyType>(const TEnumAsByte<EMarketplacePropertyType> PropertyType)
    {
   
        switch (PropertyType)
        {
        case EMarketplacePropertyType::INT_M:     return "INT";
        case EMarketplacePropertyType::STRING_M:  return "STRING";
        case EMarketplacePropertyType::ARRAY_M:   return "ARRAY";
        case EMarketplacePropertyType::GENERIC_M: return "GENERIC";
        default: return "Unknown";
        }
    }

    template<>
    FString AsString<EMarketplaceKind>(const TEnumAsByte<EMarketplaceKind> MarketplaceKind)
    {
        switch (MarketplaceKind)
        {
            case EMarketplaceKind::SEQUENCE_MARKETPLACE_V1: return "sequence_marketplace_v1";
            case EMarketplaceKind::SEQUENCE_MARKETPLACE_V2: return "sequence_marketplace_v2";
            case EMarketplaceKind::OPENSEA:                 return "opensea";
            case EMarketplaceKind::MAGIC_EDEN:              return "magic_eden";
            case EMarketplaceKind::MINTIFY:                 return "mintify";
            case EMarketplaceKind::LOOKS_RARE:              return "looks_rare";
            case EMarketplaceKind::X2Y2:                    return "x2y2";
            case EMarketplaceKind::SUDO_SWAP:               return "sudo_swap";
            case EMarketplaceKind::COINBASE:                return "coinbase";
            case EMarketplaceKind::RARIBLE:                 return "rarible";
            case EMarketplaceKind::NFTX:                    return "nftx";
            case EMarketplaceKind::FOUNDATION:              return "foundation";
            case EMarketplaceKind::MANIFOLD:                return "manifold";
            case EMarketplaceKind::ZORA:                    return "zora";
            case EMarketplaceKind::BLUR:                    return "blur";
            case EMarketplaceKind::SUPER_RARE:              return "super_rare";
            case EMarketplaceKind::OKX:                     return "okx";
            case EMarketplaceKind::ELEMENT:                 return "element";
            case EMarketplaceKind::AQUA_XYZ:                return "aqua_xyz";
            case EMarketplaceKind::AURANFT_CO:              return "auranft_co";

            default: return "Unknown";
        }
    }

    template<typename EnumType>
    static TArray<FString> EnumArrayToStringList(const TArray<TEnumAsByte<EnumType>>& EnumArray);

    // Specialization for EMarketplaceKind
    template<>
    TArray<FString> EnumArrayToStringList<EMarketplaceKind>(const TArray<TEnumAsByte<EMarketplaceKind>>& MarketplaceKinds)
    {
        TArray<FString> StringArray;
        for (const TEnumAsByte<EMarketplaceKind>& Kind : MarketplaceKinds)
        {
            StringArray.Add(AsString(Kind));
        }
        return StringArray;
    }

    // Specialization for EMarketplacePropertyType
    template<>
    TArray<FString> EnumArrayToStringList<EMarketplacePropertyType>(const TArray<TEnumAsByte<EMarketplacePropertyType>>& PropertyTypes)
    {
        TArray<FString> StringArray;
        for (const TEnumAsByte<EMarketplacePropertyType>& Type : PropertyTypes)
        {
            StringArray.Add(AsString(Type));
        }
        return StringArray;
    }
};
