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
//
//
//// PropertyType Extensions
//UCLASS()
//class UPropertyTypeExtensions : public UObject
//{
//    GENERATED_BODY()
//
//public:
//    UFUNCTION(BlueprintCallable, Category = "PropertyType")
//    static FString AsString(EPropertyType PropertyType)
//    {
//        switch (PropertyType)
//        {
//        case EPropertyType::INT:     return "INT";
//        case EPropertyType::STRING:  return "STRING";
//        case EPropertyType::ARRAY:   return "ARRAY";
//        case EPropertyType::GENERIC: return "GENERIC";
//        default: return "Unknown";
//        }
//    }
//};
//
//
//// MarketplaceKind Extensions
//UCLASS()
//class UMarketplaceKindExtensions : public UObject
//{
//    GENERATED_BODY()
//
//public:
//    UFUNCTION(BlueprintCallable, Category = "MarketplaceKind")
//    static FString AsString(EMarketplaceKind MarketplaceKind)
//    {
//        switch (MarketplaceKind)
//        {
//        case EMarketplaceKind::UNKNOWN:                return "unknown";
//        case EMarketplaceKind::SEQUENCE_MARKETPLACE_V1: return "sequence_marketplace_v1";
//        case EMarketplaceKind::SEQUENCE_MARKETPLACE_V2: return "sequence_marketplace_v2";
//        case EMarketplaceKind::OPENSEA:                return "opensea";
//        case EMarketplaceKind::MAGIC_EDEN:             return "magic_eden";
//        case EMarketplaceKind::MINTIFY:                return "mintify";
//        case EMarketplaceKind::LOOKS_RARE:             return "looks_rare";
//        case EMarketplaceKind::X2Y2:                   return "x2y2";
//        case EMarketplaceKind::SUDO_SWAP:              return "sudo_swap";
//        case EMarketplaceKind::COINBASE:               return "coinbase";
//        case EMarketplaceKind::RARIBLE:                return "rarible";
//        case EMarketplaceKind::NFTX:                   return "nftx";
//        case EMarketplaceKind::FOUNDATION:             return "foundation";
//        case EMarketplaceKind::MANIFOLD:               return "manifold";
//        case EMarketplaceKind::ZORA:                   return "zora";
//        case EMarketplaceKind::BLUR:                   return "blur";
//        case EMarketplaceKind::SUPER_RARE:             return "super_rare";
//        case EMarketplaceKind::OKX:                    return "okx";
//        case EMarketplaceKind::ELEMENT:                return "element";
//        case EMarketplaceKind::AQUA_XYZ:               return "aqua_xyz";
//        case EMarketplaceKind::AURANFT_CO:             return "auranft_co";
//
//        default: return "Unknown";
//
//        }
//    }
//};