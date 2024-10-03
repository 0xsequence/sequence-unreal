#pragma once

#include "CoreMinimal.h"
#include "Marketplace/Marketplace_Enums.h"
#include "SeqPropertyFilter.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqPropertyFilter
{
    GENERATED_USTRUCT_BODY()

public:
    // Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
    TEnumAsByte<EMarketplacePropertyType>Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
    int64 Min;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
    int64 Max;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
    TArray<FString> Values;  

    bool ContainsData()
    {
        return !Name.IsEmpty() ||
            Type != EMarketplacePropertyType() || // Assuming you have an Unknown enum value
            (Min != 0 || Max != 0) ||
            Values.Num() > 0;
    }

    // Method to get JSON representation
    TSharedPtr<FJsonObject> GetJson() const
    {
        TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
        JsonObject->SetStringField("name", Name);
        JsonObject->SetNumberField("type", (int32)Type); // Cast enum to int for JSON
        JsonObject->SetNumberField("min", Min);
        JsonObject->SetNumberField("max", Max);

        if (Values.Num() > 0)
        {
            JsonObject->SetArrayField("values", USequenceSupport::StringListToJsonArray(Values));
        }

        return JsonObject;
    }

    // Method to get arguments as a string
    FString GetArgs() const
    {
        FString Args = "";
        Args.Append("{");

        if (!Name.IsEmpty())
        {
            Args.Append("\"name\":\"" + Name + "\",");
        }

        if (Type != EMarketplacePropertyType()) // Assuming you have an Unknown enum value
        {
            Args.Append("\"type\":" + UMarketplaceEnumsExtensions::AsString(Type) + ",");
        }

        Args.Append("\"min\":" + FString::Printf(TEXT("%lld"), Min) + ",");
        Args.Append("\"max\":" + FString::Printf(TEXT("%lld"), Max) + ",");

        if (Values.Num() > 0)
        {
            Args.Append("\"values\":" + USequenceSupport::StringListToSimpleString(Values) + ",");
        }

        // Remove the last comma if any fields were added
        if (Args.EndsWith(","))
        {
            Args.RemoveAt(Args.Len() - 1);
        }

        Args.Append("}");
        return Args;
    }
};
