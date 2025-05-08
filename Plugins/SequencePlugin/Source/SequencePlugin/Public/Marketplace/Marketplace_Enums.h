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
class SEQUENCEPLUGIN_API UMarketplaceEnumsExtensions : public UObject
{
    GENERATED_BODY()

public:

    template<typename EnumType>
    static FString AsString(EnumType EnumValue);
    
    template<typename EnumType>
    static FString AsString(TEnumAsByte<EnumType> EnumValue);

    template<typename EnumType>
    static TArray<FString> EnumArrayToStringList(const TArray<TEnumAsByte<EnumType>>& EnumArray);
};
