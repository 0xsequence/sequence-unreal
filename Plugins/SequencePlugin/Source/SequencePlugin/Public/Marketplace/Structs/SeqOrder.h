#pragma once

#include "CoreMinimal.h"
#include "SeqFeeBreakdown.h"
#include "Marketplace/Marketplace_Enums.h"
#include "Util/SequenceSupport.h"
#include "SeqOrder.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqOrder
{
    GENERATED_USTRUCT_BODY()

public:

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString OrderId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    TEnumAsByte<EMarketplaceKind> Marketplace;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    TEnumAsByte<EOrderSide> Side;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    TEnumAsByte<EOrderStatus> Status;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    int64 ChainId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString CollectionContractAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString TokenId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString CreatedBy;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString PriceAmount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString PriceAmountFormatted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString PriceAmountNet;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString PriceAmountNetFormatted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString PriceCurrencyAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    int64 PriceDecimals;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    int64 PriceUSD;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString QuantityInitial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString QuantityInitialFormatted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString QuantityRemaining;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString QuantityRemainingFormatted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString QuantityAvailable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString QuantityAvailableFormatted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    int64 QuantityDecimals;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    int64 FeeBps;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    TArray<FSeqFeeBreakdown> FeeBreakdown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString ValidFrom;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString ValidUntil;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString OrderCreatedAt;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString OrderUpdatedAt;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString CreatedAt;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString UpdatedAt;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString DeletedAt;

    bool customConstructor = true;

    TSharedPtr<FJsonObject> GetJson()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable(new FJsonObject);
        ret.Get()->SetStringField("orderId", OrderId);
        ret.Get()->SetStringField("marketplace", UEnum::GetValueAsString(Marketplace.GetValue()));
        ret.Get()->SetStringField("side", UEnum::GetValueAsString(Side.GetValue()));
        ret.Get()->SetStringField("status", UEnum::GetValueAsString(Status.GetValue()));
        ret.Get()->SetNumberField("chainId", ChainId);
        ret.Get()->SetStringField("collectionContractAddress", CollectionContractAddress);
        ret.Get()->SetStringField("tokenId", TokenId);
        ret.Get()->SetStringField("createdBy", CreatedBy);
        ret.Get()->SetStringField("priceAmount", PriceAmount);
        ret.Get()->SetStringField("priceAmountFormatted", PriceAmountFormatted);
        ret.Get()->SetStringField("priceAmountNet", PriceAmountNet);
        ret.Get()->SetStringField("priceAmountNetFormatted", PriceAmountNetFormatted);
        ret.Get()->SetStringField("priceCurrencyAddress", PriceCurrencyAddress);
        ret.Get()->SetNumberField("priceDecimals", PriceDecimals);
        ret.Get()->SetNumberField("priceUSD", PriceUSD);
        ret.Get()->SetStringField("quantityInitial", QuantityInitial);
        ret.Get()->SetStringField("quantityInitialFormatted", QuantityInitialFormatted);
        ret.Get()->SetStringField("quantityRemaining", QuantityRemaining);
        ret.Get()->SetStringField("quantityRemainingFormatted", QuantityRemainingFormatted);
        ret.Get()->SetStringField("quantityAvailable", QuantityAvailable);
        ret.Get()->SetStringField("quantityAvailableFormatted", QuantityAvailableFormatted);
        ret.Get()->SetNumberField("quantityDecimals", QuantityDecimals);
        ret.Get()->SetNumberField("feeBps", FeeBps);

        // Serialize FeeBreakdown array
        TArray<TSharedPtr<FJsonValue>> feeBreakdownArray;
        for (FSeqFeeBreakdown& fee : FeeBreakdown)
        {
            feeBreakdownArray.Add(MakeShareable(new FJsonValueObject(fee.GetJson())));
        }
        ret.Get()->SetArrayField("feeBreakdown", feeBreakdownArray);

        ret.Get()->SetStringField("validFrom", ValidFrom);
        ret.Get()->SetStringField("validUntil", ValidUntil);
        ret.Get()->SetStringField("orderCreatedAt", OrderCreatedAt);
        ret.Get()->SetStringField("orderUpdatedAt", OrderUpdatedAt);
        ret.Get()->SetStringField("createdAt", CreatedAt);
        ret.Get()->SetStringField("updatedAt", UpdatedAt);
        ret.Get()->SetStringField("deletedAt", DeletedAt);
        return ret;
    }

    void Setup(FJsonObject& json_in)
    {
        OrderId = json_in.GetStringField(TEXT("orderId"));

        if (!json_in.TryGetField(TEXT("marketplace")))
            Marketplace = EMarketplaceKind(json_in.GetIntegerField(TEXT("marketplace")));

        if (!json_in.TryGetField(TEXT("side")))
            Side = EOrderSide(json_in.GetIntegerField(TEXT("side")));

        if (!json_in.TryGetField(TEXT("status")))
            Status = EOrderStatus(json_in.GetIntegerField(TEXT("Status")));

        ChainId = json_in.GetNumberField(TEXT("chainId"));
        CollectionContractAddress = json_in.GetStringField(TEXT("collectionContractAddress"));
        TokenId = json_in.GetStringField(TEXT("tokenId"));
        CreatedBy = json_in.GetStringField(TEXT("createdBy"));
        PriceAmount = json_in.GetStringField(TEXT("priceAmount"));
        PriceAmountFormatted = json_in.GetStringField(TEXT("priceAmountFormatted"));
        PriceAmountNet = json_in.GetStringField(TEXT("priceAmountNet"));
        PriceAmountNetFormatted = json_in.GetStringField(TEXT("priceAmountNetFormatted"));
        PriceCurrencyAddress = json_in.GetStringField(TEXT("priceCurrencyAddress"));
        PriceDecimals = json_in.GetNumberField(TEXT("priceDecimals"));
        PriceUSD = json_in.GetNumberField(TEXT("priceUSD"));
        QuantityInitial = json_in.GetStringField(TEXT("quantityInitial"));
        QuantityInitialFormatted = json_in.GetStringField(TEXT("quantityInitialFormatted"));
        QuantityRemaining = json_in.GetStringField(TEXT("quantityRemaining"));
        QuantityRemainingFormatted = json_in.GetStringField(TEXT("quantityRemainingFormatted"));
        QuantityAvailable = json_in.GetStringField(TEXT("quantityAvailable"));
        QuantityAvailableFormatted = json_in.GetStringField(TEXT("quantityAvailableFormatted"));
        QuantityDecimals = json_in.GetNumberField(TEXT("quantityDecimals"));
        FeeBps = json_in.GetNumberField(TEXT("feeBps"));

        if (json_in.HasField(TEXT("feeBreakdown"))) 
        {
            const TArray<TSharedPtr<FJsonValue>>& feeBreakdownArray = json_in.GetArrayField(TEXT("feeBreakdown")); 
            FeeBreakdown.Empty();
            for (int32 i = 0; i < feeBreakdownArray.Num(); ++i) 
            {
                TSharedPtr<FJsonValue> feeValue = feeBreakdownArray[i];
                if (feeValue->Type == EJson::Object)
                {
                    FSeqFeeBreakdown fee;
                    //TSharedPtr<FJsonObject> feeJsonObject = USequenceSupport::JsonStringToObject(feeValue->AsString()); 
                    fee.Setup(*feeValue->AsObject()); 
                    FeeBreakdown.Add(fee);

                    UE_LOG(LogTemp, Warning, TEXT("fee setup"));
                }
            }
        }

        ValidFrom = json_in.GetStringField(TEXT("validFrom"));
        ValidUntil = json_in.GetStringField(TEXT("validUntil"));
        OrderCreatedAt = json_in.GetStringField(TEXT("orderCreatedAt"));
        OrderUpdatedAt = json_in.GetStringField(TEXT("orderUpdatedAt"));
        CreatedAt = json_in.GetStringField(TEXT("createdAt"));
        UpdatedAt = json_in.GetStringField(TEXT("updatedAt"));
        DeletedAt = json_in.GetStringField(TEXT("deletedAt"));
    }
};