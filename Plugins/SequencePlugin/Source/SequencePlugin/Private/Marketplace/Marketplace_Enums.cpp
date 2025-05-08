#include "Marketplace/Marketplace_Enums.h"

template<>
FString UMarketplaceEnumsExtensions::AsString<EOrderbookKind>(const EOrderbookKind OrderbookKind)
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

template<>
FString UMarketplaceEnumsExtensions::AsString<EMarketplacePropertyType>(const TEnumAsByte<EMarketplacePropertyType> PropertyType)
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
FString UMarketplaceEnumsExtensions::AsString<EMarketplaceKind>(const TEnumAsByte<EMarketplaceKind> MarketplaceKind)
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

// Specialization for EMarketplaceKind
template<>
TArray<FString> UMarketplaceEnumsExtensions::EnumArrayToStringList<EMarketplaceKind>(const TArray<TEnumAsByte<EMarketplaceKind>>& MarketplaceKinds)
{
	TArray<FString> StringArray;
	for (const TEnumAsByte<EMarketplaceKind>& Kind : MarketplaceKinds)
	{
		StringArray.Add(UMarketplaceEnumsExtensions::AsString(Kind));
	}
	return StringArray;
}

// Specialization for EMarketplacePropertyType
template<>
TArray<FString> UMarketplaceEnumsExtensions::EnumArrayToStringList<EMarketplacePropertyType>(const TArray<TEnumAsByte<EMarketplacePropertyType>>& PropertyTypes)
{
	TArray<FString> StringArray;
	for (const TEnumAsByte<EMarketplacePropertyType>& Type : PropertyTypes)
	{
		StringArray.Add(UMarketplaceEnumsExtensions::AsString(Type));
	}
	return StringArray;
}
