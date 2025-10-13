#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "SequencePlugin/Public/Util/ByteArrayUtils.h"
#include "Util/SequenceCoder.h"
#include "Util/SequenceSupport.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEncodeSignedInt, "SequencePlugin.UnitTests.CoderTests.EncodeSignedInt", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FEncodeSignedInt::RunTest(const FString& Parameters)
{
	const FString ExpectedHex = "0x0000000000000000000000000000000000000000000000000000000000000003";
	const FString Value = "3";

	const TArray<uint8> Bytes = FSequenceCoder::EncodeSignedInt(Value);
	const FString EncodedBytes = FByteArrayUtils::BytesToHexString(Bytes);
    
	UE_LOG(LogTemp, Display, TEXT("Encoded signed int: %s = %s"), *EncodedBytes, *ExpectedHex)
    
	TestEqual("Hex strings should match", EncodedBytes, ExpectedHex);
    
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEncodeBoolean, "SequencePlugin.UnitTests.CoderTests.EncodeBoolean", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FEncodeBoolean::RunTest(const FString& Parameters)
{
	const FString ExpectedHex = "0x0000000000000000000000000000000000000000000000000000000000000001";
	constexpr bool Value = true;

	const TArray<uint8> Bytes = FSequenceCoder::EncodeBoolean(Value);
	const FString EncodedBytes = FByteArrayUtils::BytesToHexString(Bytes);
    
	UE_LOG(LogTemp, Display, TEXT("Encoded boolean: %s = %s"), *EncodedBytes, *ExpectedHex)
    
	TestEqual("Hex strings should match", EncodedBytes, ExpectedHex);
    
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEncodeAddress, "SequencePlugin.UnitTests.CoderTests.EncodeAddress", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FEncodeAddress::RunTest(const FString& Parameters)
{
	const FString ExpectedHex = "0x00000000000000000000000033985d320809e26274a72e03268c8a29927bc6da";
	const FString Value = "0x33985d320809E26274a72E03268c8a29927Bc6dA";

	const TArray<uint8> Bytes = FSequenceCoder::EncodeAddress(Value);
	const FString EncodedBytes = FByteArrayUtils::BytesToHexString(Bytes);
    
	UE_LOG(LogTemp, Display, TEXT("Encoded address: %s = %s"), *EncodedBytes, *ExpectedHex)
    
	TestEqual("Hex strings should match", EncodedBytes, ExpectedHex);
    
	return true;
}
