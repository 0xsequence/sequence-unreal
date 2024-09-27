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
    TArray<FString> Marketplaces; 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    TArray<FString> InAccounts;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    TArray<FString> NotInAccounts;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    TArray<FString> OrdersCreatedBy;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    TArray<FString> OrdersNotCreatedBy;

    bool ContainsData()
    {
        // Check for valid data in the properties
        return bIncludeEmpty ||
            !SearchText.IsEmpty() ||
            Properties.Num() > 0 ||
            Marketplaces.Num() > 0 ||
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
                    propertiesList.Add(property.GetArgs()); // Assuming GetArgs() exists in FSeqPropertyFilter
                }
                ret.Append(USequenceSupport::StringListToSimpleString(propertiesList));
            }

            // Add marketplaces
            if (Marketplaces.Num() > 0)
            {
                ret.Append(",\"marketplaces\":");
                ret.Append(USequenceSupport::StringListToSimpleString(Marketplaces));
            }

            // Add in accounts
            if (InAccounts.Num() > 0)
            {
                ret.Append(",\"inAccounts\":");
                ret.Append(USequenceSupport::StringListToSimpleString(InAccounts));
            }

            // Add not in accounts
            if (NotInAccounts.Num() > 0)
            {
                ret.Append(",\"notInAccounts\":");
                ret.Append(USequenceSupport::StringListToSimpleString(NotInAccounts));
            }

            // Add orders created by
            if (OrdersCreatedBy.Num() > 0)
            {
                ret.Append(",\"ordersCreatedBy\":");
                ret.Append(USequenceSupport::StringListToSimpleString(OrdersCreatedBy));
            }

            // Add orders not created by
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
                // Assuming FSeqPropertyFilter has a method to convert to JSON
                propertyJson = property.GetJson(); // Assuming GetJson() exists
                propertiesJsonArray.Add(MakeShareable(new FJsonValueObject(propertyJson)));
            }
            ret->SetArrayField("properties", propertiesJsonArray);
        }

        if (Marketplaces.Num() > 0)
        {
            ret->SetArrayField("marketplaces", USequenceSupport::StringListToJsonArray(Marketplaces));
        }

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

    void Setup(FJsonObject json_in)
    {
        // Implement parsing from JSON here if needed
        // Example: bIncludeEmpty = json_in->GetBoolField("includeEmpty");
    }
};