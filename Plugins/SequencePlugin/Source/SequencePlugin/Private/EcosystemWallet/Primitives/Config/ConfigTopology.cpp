#include "ConfigTopology.h"

#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"

FConfigTopology FromServiceConfigTree(const FString& Input)
{
    /*if (Input.StartsWith("["))
    {
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Input);
        TArray<TSharedPtr<FJsonValue>> List;
        if (!FJsonSerializer::Deserialize(Reader, List) || List.Num() != 2)
        {
            UE_LOG(LogTemp, Error, TEXT("Invalid node structure in JSON"));
            throw std::runtime_error("Invalid node structure in JSON");
        }

        return FConfigTopology(
            MakeShared<Node>(
                FromServiceConfigTree(List[0]->AsString()),
                FromServiceConfigTree(List[1]->AsString())
            )
        );
    }

    // Handle hex string "0x..."
    if (Input.StartsWith("0x"))
    {
        NodeLeaf Leaf;
        Leaf.Value = HexStringToByteArray(Input); // you’ll need to implement HexStringToByteArray
        return Leaf.ToTopology();
    }

    // Handle JSON object
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Input);
    TSharedPtr<FJsonObject> Obj;
    if (!FJsonSerializer::Deserialize(Reader, Obj) || !Obj.IsValid())
    {
        throw std::runtime_error("Invalid JSON object");
    }

    // "weight" case
    if (Obj->HasField("weight"))
    {
        FString WeightStr = Obj->GetStringField("weight");
        BigInt Weight = ParseBigInt(WeightStr); // You’ll need to wrap BigInteger equivalent in C++

        if (Obj->HasField("address"))
        {
            FString AddressStr = Obj->GetStringField("address");

            if (Obj->HasField("imageHash"))
            {
                FString ImageHash = Obj->GetStringField("imageHash");
                SapientSignerLeaf Leaf;
                Leaf.address = Address(AddressStr);
                Leaf.weight = Weight;
                Leaf.imageHash = ImageHash;
                return Leaf.ToTopology();
            }

            SignerLeaf Leaf;
            Leaf.address = Address(AddressStr);
            Leaf.weight = Weight;
            return Leaf.ToTopology();
        }

        if (Obj->HasField("tree"))
        {
            NestedLeaf Leaf;
            Leaf.weight = Weight;
            Leaf.threshold = ParseBigInt(Obj->GetStringField("threshold"));
            Leaf.tree = FromServiceConfigTree(Obj->GetStringField("tree"));
            return Leaf.ToTopology();
        }
    }

    // "subdigest" case
    if (Obj->HasField("subdigest"))
    {
        FString SubdigestStr = Obj->GetStringField("subdigest");
        TArray<uint8> Subdigest = HexStringToByteArray(SubdigestStr);

        bool bIsAny = false;
        if (Obj->HasField("isAnyAddress"))
        {
            bIsAny = Obj->GetBoolField("isAnyAddress");
        }

        if (bIsAny)
        {
            AnyAddressSubdigestLeaf Leaf;
            Leaf.digest = Subdigest;
            return Leaf.ToTopology();
        }

        SubdigestLeaf Leaf;
        Leaf.digest = Subdigest;
        return Leaf.ToTopology();
    }

    FString Err = FString::Printf(TEXT("Unknown config tree '%s'"), *Input);
    throw std::runtime_error(TCHAR_TO_UTF8(*Err));*/
    return FConfigTopology();
}
