#include "CoreMinimal.h"
#include "EcosystemWallet/Primitives/Sessions/SessionsTopology.h"
#include "Misc/AutomationTest.h"
#include "Util/SequenceSupport.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSessionsTopologyFromTree, "SequencePlugin.UnitTests.EcosystemWallet.SessionsTopologyFromTree", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FSessionsTopologyFromTree::RunTest(const FString& Parameters)
{
    const FString ServiceTree = "[[{\"data\":\"0x03\"},{\"data\":\"0x04957640f9a397aefeaf83a61c1360627658e15e70\"}],[{\"data\":\"0x04764b75df6c49d4decdc0e331066cb077f55de0cd\"},{\"data\":\"0x00a50bc8f9e68846e9fedab74b544f71fc2439d1ad0000000000000000000000000000000000000000000000000000000000066eee000000000000000000000000000000000000000000000000000000000000000000000199b5b155d00133985d320809e26274a72e03268c8a29927bc6da00\"}]]";
    const FString ExpectedIdentitySigner = "0x957640f9a397aefeaf83a61c1360627658e15e70";
    
    const TSharedPtr<FJsonValue> JsonValue = USequenceSupport::ParseJsonValue(ServiceTree);
    const TSharedPtr<FSessionsTopology> SessionsTopology = FSessionsTopology::FromServiceConfigTree(JsonValue);
    
    if (!SessionsTopology.IsValid())
    {
        UE_LOG(LogTemp, Display, TEXT("Failed to parse topology from service tree"));
        return false;
    }

    const FString IdentitySigner = SessionsTopology.Get()->FindIdentitySigner();

    if (IdentitySigner == "")
    {
        UE_LOG(LogTemp, Display, TEXT("Unable to find identity signer in topology"));
        return false;
    }
    
    TestEqual("Identity signers should match", IdentitySigner, ExpectedIdentitySigner);
    
    return true;
} 