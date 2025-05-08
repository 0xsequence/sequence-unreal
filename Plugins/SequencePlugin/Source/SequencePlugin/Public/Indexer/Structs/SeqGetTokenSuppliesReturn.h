#pragma once

#include "CoreMinimal.h"
#include "SeqPage.h"
#include "Indexer/Indexer_Enums.h"
#include "SeqTokenSupply.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "SeqGetTokenSuppliesReturn.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetTokenSuppliesReturn
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FSeqPage page;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        TEnumAsByte<EContractType> contractType = EContractType::ERC1155;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        TArray<FSeqTokenSupply> tokenIDs;
    bool customConstructor = false;//used to tell buildresponse whether or not to use a custom constructor OR the unreal one!
    void construct(FJsonObject json_in) {};//dummy construct for templating

    /*
    * Used to return a json object formed by this struct used for
    * args & testing purposes
    */
    TSharedPtr<FJsonObject> GetJson()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);

        ret.Get()->SetObjectField("page", page.GetJson());
        ret.Get()->SetStringField("contractType", UEnum::GetValueAsString(contractType.GetValue()));
        TArray<TSharedPtr<FJsonObject>> jList;

        for (FSeqTokenSupply tID : tokenIDs)
        {
            jList.Add(tID.GetJson());
        }
        ret.Get()->SetStringField("tokenIDs", USequenceSupport::JsonObjListToSimpleString(jList));
        return ret;
    }

    /*
    * Used to Handle Edge Cases with Unreal's Json parsing
    */
    void Setup(FJsonObject json_in)
    {
        const TArray<TSharedPtr<FJsonValue>>* lst;
        if (json_in.TryGetArrayField(TEXT("tokenIDs"),lst))
        {
            for (int32 i = 0; i < tokenIDs.Num(); i++)
            {
                const TSharedPtr<FJsonObject>* itemObj;
                if ((*lst)[i].Get()->TryGetObject(itemObj))
                {
                    tokenIDs[i].setup(*itemObj->Get());
                }//if
            }//for
        }//if
    }//setup
};