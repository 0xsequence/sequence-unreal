#include "CoreMinimal.h"
#include "EcosystemWallet/Primitives/Config/ConfigTopology.h"
#include "EcosystemWallet/Primitives/Config/Leafs/ConfigSapientSignerLeaf.h"
#include "Misc/AutomationTest.h"
#include "Util/SequenceSupport.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FConfigFromServiceTree, "SequencePlugin.UnitTests.EcosystemWallet.ConfigFromServiceTree", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FConfigFromServiceTree::RunTest(const FString& Parameters)
{
    const FString ServiceTree = "[[[{\"address\":\"0x957640f9a397AefEaF83A61C1360627658E15e70\",\"weight\":1},{\"address\":\"0x764B75DF6c49D4dEcDC0E331066CB077f55de0cd\",\"weight\":1}],{\"threshold\":1,\"tree\":{\"address\":\"0xa2e70CeaB3Eb145F32d110383B75B330fA4e288a\",\"weight\":1},\"weight\":1}],[{\"threshold\":2,\"tree\":[{\"address\":\"0x0000000000CC58810c33F3a0D78aA1Ed80FaDcD8\",\"imageHash\":\"0x37cb32e4a4c3a001c0f82405b8027d651336133fe14545190dc4d10292dddf38\",\"weight\":1},{\"threshold\":1,\"tree\":{\"address\":\"0x18002Fc09deF9A47437cc64e270843dE094f5984\",\"weight\":1},\"weight\":1}],\"weight\":255},{\"address\":\"0x0000000000213697bCA95E7373787a40858a51C7\",\"imageHash\":\"0xd7a38130000e64f4afe9e2a612a564643bcd27fae713c963a3420be42f293894\",\"weight\":255}]]";
    const FString SessionsManagerAddress = "0x0000000000CC58810c33F3a0D78aA1Ed80FaDcD8";
    const FString SessionsImageHash = "0x37cb32e4a4c3a001c0f82405b8027d651336133fe14545190dc4d10292dddf38";
    
    const TSharedPtr<FJsonValue> JsonValue = USequenceSupport::ParseJsonValue(ServiceTree);
    const TSharedPtr<FConfigTopology> ConfigTopology = FConfigTopology::FromServiceConfigTree(JsonValue);
    
    if (!ConfigTopology.IsValid())
    {
        UE_LOG(LogTemp, Display, TEXT("Failed to parse topology from service tree"));
        return false;
    }

    TSharedPtr<FConfigLeaf> Leaf = ConfigTopology.Get()->FindSignerLeaf(SessionsManagerAddress);
    if (!Leaf.IsValid())
    {
        UE_LOG(LogTemp, Display, TEXT("Failed to find signer leaf in topology"));
        return false;
    }

    const FConfigSapientSignerLeaf* SapientSignerLeaf = static_cast<FConfigSapientSignerLeaf*>(Leaf.Get());
    const FString SessionsLeafImageHash = SapientSignerLeaf->ImageHash;
    
    TestEqual("Hex strings should match", SessionsLeafImageHash, SessionsImageHash);
    
    return true;
} 