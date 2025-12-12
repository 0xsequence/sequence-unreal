#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "SequencePlugin/Public/Util/ByteArrayUtils.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHexStringToBytes, "SequencePlugin.UnitTests.TypesTests.HexStringToBytes", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FHexStringToBytes::RunTest(const FString& Parameters)
{
    const FString HexString = "0x1234567890123456789012345678901234567890";

    TArray<uint8> Bytes = FByteArrayUtils::HexStringToBytes(HexString);

    const FString NewHexString = FByteArrayUtils::BytesToHexString(Bytes);

    UE_LOG(LogTemp, Display, TEXT("Encoded hex: %s"), *NewHexString)
    
    TestEqual("Hex strings should match", HexString, NewHexString);
    
    return true;
} 