#include "ConfigTopology.h"
#include "Dom/JsonObject.h"
#include "Leafs/ConfigAnyAddressSubdigestLeaf.h"
#include "Leafs/ConfigNestedLeaf.h"
#include "Leafs/ConfigNodeLeaf.h"
#include "Leafs/ConfigSapientSignerLeaf.h"
#include "Leafs/ConfigSignerLeaf.h"
#include "Leafs/ConfigSubdigestLeaf.h"
#include "ConfigNode.h"
#include "ConfigLeaf.h"

TSharedPtr<FConfigLeaf> FConfigTopology::FindSignerLeaf(const FString& Address) const
{
    if (IsNode())
    {
        if (Node->Left.IsValid() && (Node->Left.Get()->IsLeaf() || Node->Left.Get()->IsNode()))
        {
            TSharedPtr<FConfigLeaf> LeftResult = Node->Left.Get()->FindSignerLeaf(Address);
            if (LeftResult != nullptr)
            {
                return LeftResult;
            }
        }
        if (Node->Right.IsValid() && (Node->Right.Get()->IsLeaf() || Node->Right.Get()->IsNode()))
        {
            return Node->Right.Get()->FindSignerLeaf(Address);
        }
    }

    if (IsLeaf())
    {
        switch (Leaf->Type)
        {
        case EConfigLeafType::Nested:
            {
                auto* Nested = static_cast<FConfigNestedLeaf*>(Leaf.Get());
                if (Nested->Tree.IsValid())
                {
                    return Nested->Tree->FindSignerLeaf(Address);
                }
                
                return nullptr;
            }

        case EConfigLeafType::Signer:
            {
                const auto* SignerLeaf = static_cast<const FConfigSignerLeaf*>(Leaf.Get());
                return SignerLeaf->Address.Equals(Address) ? Leaf : nullptr;
            }

        case EConfigLeafType::SapientSigner:
            {
                const auto* SapientSignerLeaf = static_cast<const FConfigSapientSignerLeaf*>(Leaf.Get());
                return SapientSignerLeaf->Address.Equals(Address) ? Leaf : nullptr;
            }

        default:
            return nullptr;
        }
    }

    return nullptr;
}

TSharedPtr<FConfigTopology> FConfigTopology::FromServiceConfigTree(const TSharedPtr<FJsonValue>& Input)
{
    if (Input->Type == EJson::Array)
    {
        TArray<TSharedPtr<FJsonValue>> List = Input->AsArray();
        
        return MakeShared<FConfigTopology>(FConfigTopology(
            MakeShared<FConfigNode>(
                FromServiceConfigTree(List[0]),
                FromServiceConfigTree(List[1]))));
    }

    if (Input->Type == EJson::String && Input->AsString().StartsWith("0x"))
    {
        return MakeShared<FConfigTopology>(FConfigTopology(MakeShared<FConfigNodeLeaf>(FConfigNodeLeaf(Input->AsString()))));
    }
    
    if (Input->Type != EJson::Object)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid JSON object"));
        return MakeShared<FConfigTopology>(FConfigTopology());
    }

    TSharedPtr<FJsonObject> Obj = Input->AsObject();

    if (Obj->HasField(TEXT("weight")))
    {
        FString WeightStr = Obj->GetStringField(TEXT("weight"));

        if (Obj->HasField(TEXT("address")))
        {
            FString AddressStr = Obj->GetStringField(TEXT("address"));

            if (Obj->HasField(TEXT("imageHash")))
            {
                FString ImageHash = Obj->GetStringField(TEXT("imageHash"));
                return MakeShared<FConfigTopology>(FConfigTopology(MakeShared<FConfigSapientSignerLeaf>(FConfigSapientSignerLeaf(AddressStr, WeightStr, ImageHash))));
            }

            return MakeShared<FConfigTopology>(FConfigTopology(MakeShared<FConfigSignerLeaf>(FConfigSignerLeaf(AddressStr, WeightStr))));
        }

        if (Obj->HasField(TEXT("tree")))
        {
            const TSharedPtr<FConfigTopology> TreeTopology = FromServiceConfigTree(Obj->TryGetField(TEXT("tree")));
            return MakeShared<FConfigTopology>(MakeShared<FConfigNestedLeaf>(FConfigNestedLeaf(WeightStr,Obj->GetStringField(TEXT("threshold")), TreeTopology)));
        }
    }

    if (Obj->HasField(TEXT("subdigest")))
    {
        const FString Subdigest = Obj->GetStringField(TEXT("subdigest"));

        bool bIsAny = false;
        if (Obj->HasField(TEXT("isAnyAddress")))
        {
            bIsAny = Obj->GetBoolField(TEXT("isAnyAddress"));
        }

        if (bIsAny)
        {
            return MakeShared<FConfigTopology>(FConfigTopology(MakeShared<FConfigAnyAddressSubdigestLeaf>(FConfigAnyAddressSubdigestLeaf(Subdigest))));
        }

        return MakeShared<FConfigTopology>(FConfigTopology(MakeShared<FConfigSubdigestLeaf>(FConfigSubdigestLeaf(Subdigest))));
    }

    UE_LOG(LogTemp, Error, TEXT("Unknown config tree"));
    return MakeShared<FConfigTopology>(FConfigTopology());
}
