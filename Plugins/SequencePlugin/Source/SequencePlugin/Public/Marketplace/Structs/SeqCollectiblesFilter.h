#pragma once

#include "CoreMinimal.h"
#include "Util/SequenceSupport.h"
#include "SeqPropertyFilter.h"  
#include "Marketplace/Marketplace_Enums.h"
#include "SeqCollectiblesFilter.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqCollectiblesFilter
{
    GENERATED_USTRUCT_BODY()

public:
    // Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    bool bIncludeEmpty =true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    FString SearchText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    TArray<FSeqPropertyFilter> Properties;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    TArray<TEnumAsByte<EMarketplaceKind>> MarketplaceKinds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    TArray<FString> InAccounts;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    TArray<FString> NotInAccounts;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    TArray<FString> OrdersCreatedBy;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    TArray<FString> OrdersNotCreatedBy;

    static FSeqCollectiblesFilter Empty()
    {
        return FSeqCollectiblesFilter{};
    }

    bool ContainsData()
    {
        // Check for valid data in the properties
        return bIncludeEmpty ||
            !SearchText.IsEmpty() ||
            Properties.Num() > 0 ||
            MarketplaceKinds.Num() > 0 ||
            InAccounts.Num() > 0 ||
            NotInAccounts.Num() > 0 ||
            OrdersCreatedBy.Num() > 0 ||
            OrdersNotCreatedBy.Num() > 0;
    }

    FString GetArgs()
    {
        FString ret = "";

        if (ContainsData())
        {
            ret.Append("{");
            ret.Append("\"searchText\":\"");
            ret.Append(SearchText);
            ret.Append("\",");
            
            // Add include empty field
            ret.Append("\"includeEmpty\":");
            ret.Append(bIncludeEmpty ? "true" : "false");

            // Add properties
            if (Properties.Num() > 0)
            {
                ret.Append(",\"properties\":");
                TArray<FString> propertiesList;
                for (const FSeqPropertyFilter& property : Properties)
                {
                    propertiesList.Add(property.GetArgs());
                }
                ret.Append(USequenceSupport::StringListToSimpleString(propertiesList));
            }

            if (MarketplaceKinds.Num() > 0)
            {
                ret.Append(",\"marketplaces\":[");

                for (int32 i = 0; i < MarketplaceKinds.Num(); i++)
                {
                    ret.Append("\"");
                    ret.Append(UMarketplaceEnumsExtensions::AsString(MarketplaceKinds[i]));
                    ret.Append("\"");

                    if (i < MarketplaceKinds.Num() - 1)
                    {
                        ret.Append(",");
                    }
                }

                ret.Append("]");
            }

            if (InAccounts.Num() > 0)
            {
                ret.Append(",\"inAccounts\":");
                ret.Append(USequenceSupport::StringListToSimpleString(InAccounts));
            }

            if (NotInAccounts.Num() > 0)
            {
                ret.Append(",\"notInAccounts\":");
                ret.Append(USequenceSupport::StringListToSimpleString(NotInAccounts));
            }

            if (OrdersCreatedBy.Num() > 0)
            {
                ret.Append(",\"ordersCreatedBy\":");
                ret.Append(USequenceSupport::StringListToSimpleString(OrdersCreatedBy));
            }

            if (OrdersNotCreatedBy.Num() > 0)
            {
                ret.Append(",\"ordersNotCreatedBy\":");
                ret.Append(USequenceSupport::StringListToSimpleString(OrdersNotCreatedBy));
            }

            ret.Append("}");
        }
        return ret;
    }

    TSharedPtr<FJsonObject> GetJson()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);
        ret->SetBoolField("includeEmpty", bIncludeEmpty);
        ret->SetStringField("searchText", SearchText);

        if (Properties.Num() > 0)
        {
            TArray<TSharedPtr<FJsonValue>> propertiesJsonArray;
            for (const FSeqPropertyFilter& property : Properties)
            {
                TSharedPtr<FJsonObject> propertyJson = MakeShareable<FJsonObject>(new FJsonObject);
                propertyJson = property.GetJson();
                propertiesJsonArray.Add(MakeShareable(new FJsonValueObject(propertyJson)));
            }
            ret->SetArrayField("properties", propertiesJsonArray);
        }

        ret->SetArrayField("marketplaces", USequenceSupport::StringListToJsonArray(UMarketplaceEnumsExtensions::EnumArrayToStringList<EMarketplaceKind>(MarketplaceKinds)));

        if (InAccounts.Num() > 0)
        {
            ret->SetArrayField("inAccounts", USequenceSupport::StringListToJsonArray(InAccounts));
        }

        if (NotInAccounts.Num() > 0)
        {
            ret->SetArrayField("notInAccounts", USequenceSupport::StringListToJsonArray(NotInAccounts));
        }

        if (OrdersCreatedBy.Num() > 0)
        {
            ret->SetArrayField("ordersCreatedBy", USequenceSupport::StringListToJsonArray(OrdersCreatedBy));
        }

        if (OrdersNotCreatedBy.Num() > 0)
        {
            ret->SetArrayField("ordersNotCreatedBy", USequenceSupport::StringListToJsonArray(OrdersNotCreatedBy));
        }

        return ret;
    }

};