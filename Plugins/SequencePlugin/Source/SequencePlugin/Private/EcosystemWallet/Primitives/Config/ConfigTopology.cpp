#include "ConfigTopology.h"
#include "Dom/JsonObject.h"
#include "Leafs/ConfigAnyAddressSubdigestLeaf.h"
#include "Leafs/ConfigNestedLeaf.h"
#include "Leafs/ConfigNodeLeaf.h"
#include "Leafs/ConfigSapientSignerLeaf.h"
#include "Leafs/ConfigSignerLeaf.h"
#include "Leafs/ConfigSubdigestLeaf.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "ConfigNode.h"
#include "ConfigLeaf.h"

FConfigLeaf* FConfigTopology::FindSignerLeaf(const FString& Address) const
{
    if (IsNode())
    {
        if (Node == nullptr)
        {
            return nullptr;
        }

        if (Node->Left->IsLeaf() || Node->Left->IsNode())
        {
            FConfigLeaf* LeftResult = Node->Left->FindSignerLeaf(Address);
            if (LeftResult != nullptr)
            {
                return LeftResult;
            }
        }

        if (Node->Right->IsLeaf() || Node->Right->IsNode())
        {
            return Node->Right->FindSignerLeaf(Address);
        }
    }

    if (IsLeaf() && Leaf != nullptr)
    {
        if (const FConfigNestedLeaf* NestedLeaf = static_cast<FConfigNestedLeaf*>(Leaf))
        {
            if (NestedLeaf->Tree)
            {
                return NestedLeaf->Tree->FindSignerLeaf(Address);
            }
        }

        if (const FConfigSignerLeaf* SignerLeaf = static_cast<FConfigSignerLeaf*>(Leaf))
        {
            if (SignerLeaf->Address.Equals(Address))
            {
                return Leaf;
            }
        }

        if (const FConfigSapientSignerLeaf* SapientLeaf = static_cast<FConfigSapientSignerLeaf*>(Leaf))
        {
            if (SapientLeaf->Address.Equals(Address))
            {
                return Leaf;
            }
        }
    }

    return nullptr;
}

TSharedPtr<FConfigTopology> FConfigTopology::FromServiceConfigTree(const FString& Input)
{
    if (Input.StartsWith("["))
    {
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Input);
        TArray<TSharedPtr<FJsonValue>> List;
        if (!FJsonSerializer::Deserialize(Reader, List) || List.Num() != 2)
        {
            UE_LOG(LogTemp, Error, TEXT("Invalid node structure in JSON"));
            throw std::runtime_error("Invalid node structure in JSON");
        }

        return MakeShared<FConfigTopology>(FConfigTopology(FConfigNode(FromServiceConfigTree(List[0]->AsString()),
                FromServiceConfigTree(List[1]->AsString()))));
    }

    if (Input.StartsWith("0x"))
    {
        return MakeShared<FConfigTopology>(FConfigTopology(FConfigNodeLeaf(Input)));
    }

    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Input);
    TSharedPtr<FJsonObject> Obj;
    if (!FJsonSerializer::Deserialize(Reader, Obj) || !Obj.IsValid())
    {
        throw std::runtime_error("Invalid JSON object");
    }

    if (Obj->HasField(TEXT("weight")))
    {
        FString WeightStr = Obj->GetStringField(TEXT("weight"));

        if (Obj->HasField(TEXT("address")))
        {
            FString AddressStr = Obj->GetStringField(TEXT("address"));

            if (Obj->HasField(TEXT("imageHash")))
            {
                FString ImageHash = Obj->GetStringField(TEXT("imageHash"));
                return MakeShared<FConfigTopology>(FConfigTopology(FConfigSapientSignerLeaf(AddressStr, WeightStr, ImageHash)));
            }

            return MakeShared<FConfigTopology>(FConfigTopology(FConfigSignerLeaf(AddressStr, WeightStr)));
        }

        if (Obj->HasField(TEXT("tree")))
        {
            return MakeShared<FConfigTopology>(FConfigNestedLeaf(WeightStr, Obj->GetStringField(TEXT("threshold")), FromServiceConfigTree(Obj->GetStringField(TEXT("tree")))));
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
            return MakeShared<FConfigTopology>(FConfigTopology(FConfigAnyAddressSubdigestLeaf(Subdigest)));
        }

        return MakeShared<FConfigTopology>(FConfigTopology(FConfigSubdigestLeaf(Subdigest)));
    }

    FString Err = FString::Printf(TEXT("Unknown config tree '%s'"), *Input);
    return MakeShared<FConfigTopology>(FConfigTopology());
}
