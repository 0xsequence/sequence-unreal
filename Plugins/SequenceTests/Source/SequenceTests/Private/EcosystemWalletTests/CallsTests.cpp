#include "CoreMinimal.h"
#include "EcosystemWallet/Primitives/Calls/Calls.h"
#include "EcosystemWallet/Primitives/Calls/CallTypedData.h"
#include "EcosystemWallet/Primitives/Calls/CallTypedDataFactory.h"
#include "Misc/AutomationTest.h"
#include "Util/ByteArrayUtils.h"
#include "Util/SequenceSupport.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEncodeCalls, "SequencePlugin.UnitTests.EcosystemWallet.CallsTests.EncodeCalls", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FEncodeCalls::RunTest(const FString& Parameters)
{
    const FString ExpectedHex = "0x13224433985d320809e26274a72e03268c8a29927bc6da0000040c0340fc";
    
    TArray<FCall> Calls {
        FCall {
            TEXT("0x33985d320809E26274a72E03268c8a29927Bc6dA"),
            TEXT("0x0c0340fc"),
            FBigInt("0"),
            FBigInt("0"),
            false,
            false,
            "1"
        }
    };
    
    FCalls Payload {
        Calls,
        FBigInt("0"),
        FBigInt("34"),
    };

    const FString EncodedCalls = FByteArrayUtils::BytesToHexString(Payload.Encode());

    UE_LOG(LogTemp, Log, TEXT("Encoded Calls: %s == %s"), *EncodedCalls, *ExpectedHex);
    
    TestEqual("Hex values should match", EncodedCalls, ExpectedHex);
    
    return true;
}
