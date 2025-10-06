#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "SequencePlugin/Public/Util/ByteArrayUtils.h"
#include "Util/SequenceSupport.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEncodeBigInteger, "SequencePlugin.UnitTests.TypesTests.EncodeBigInt", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FEncodeBigInteger::RunTest(const FString& Parameters)
{
    const FString ExpectedEncodedHexString = "0x09184e72a000";
    const FString BigIntValue = "10000000000000";

    const TArray<uint8> Bytes = USequenceSupport::EncodeBigInteger(BigIntValue);
    const FString EncodedBytes = FByteArrayUtils::BytesToHexString(Bytes);
    
    UE_LOG(LogTemp, Display, TEXT("BigInt encoded as hex: %s = %s"), *BigIntValue, *EncodedBytes)
    
    TestEqual("Hex strings should match", EncodedBytes, ExpectedEncodedHexString);
    
    return true;
} 