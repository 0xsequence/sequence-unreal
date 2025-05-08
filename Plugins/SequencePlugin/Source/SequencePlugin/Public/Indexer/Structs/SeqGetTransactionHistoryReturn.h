#pragma once

#include "CoreMinimal.h"
#include "SeqPage.h"
#include "SeqTransaction.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Util/Log.h"
#include "SeqGetTransactionHistoryReturn.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetTransactionHistoryReturn
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    FSeqPage page;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        TArray<FSeqTransaction> transactions;
    bool customConstructor = true;//used to tell buildresponse whether or not to use a custom constructor OR the unreal one!
    void construct(FJsonObject json_in) {};//dummy construct for templating

    /*
    * Used to get the json object formed by this struct, used for
    * args & testing purposes
    */
    TSharedPtr<FJsonObject> GetJson()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);
        ret.Get()->SetObjectField("page", page.GetJson());

        TArray<TSharedPtr<FJsonObject>> jList;
        for (FSeqTransaction tItem : transactions)
        {
            jList.Add(tItem.GetJson());
        }
        ret.Get()->SetStringField("transactions", USequenceSupport::JsonObjListToSimpleString(jList));
        return ret;
    }

    /*
    * Used to Handle Edge Cases with Unreal's Json parsing
    */
    void Setup(FJsonObject json_in)
    {
        const TSharedPtr<FJsonObject>* pageObj;

        if (json_in.TryGetObjectField(TEXT("page"), pageObj))
        {
            if (!FJsonObjectConverter::JsonObjectToUStruct((*pageObj).ToSharedRef(), &page))
            {
                SEQ_LOG(Warning, TEXT("Failed to convert page object to FSeqPage"));
            }
        }
        else
        {
            SEQ_LOG(Warning, TEXT("No page information found in the JSON."));
        }

        const TArray<TSharedPtr<FJsonValue>>* lst;
        if (json_in.TryGetArrayField(TEXT("transactions"), lst))
        {
            if (lst->Num() == 0)
            {
                SEQ_LOG(Warning, TEXT("No transactions found."));
            }
            else
            {
                transactions.SetNum(lst->Num());

                for (int i = 0; i < lst->Num(); i++)
                {
                    const TSharedPtr<FJsonObject>* itemObj;
                    if ((*lst)[i]->TryGetObject(itemObj))
                    {
                        if (itemObj && itemObj->IsValid())
                        {
                            transactions[i].setup(*itemObj->Get());
                        }
                    }
                }
            }
        }
    }
};