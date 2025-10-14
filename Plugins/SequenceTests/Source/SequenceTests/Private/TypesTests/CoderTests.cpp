#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "SequencePlugin/Public/Util/ByteArrayUtils.h"
#include "Util/SequenceCoder.h"
#include "Util/SequenceSupport.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCeckKeccakHash, "SequencePlugin.UnitTests.CoderTests.CheckKeccakHash", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FCeckKeccakHash::RunTest(const FString& Parameters)
{
	const FString Input = "0x53657175656e63652073617069656e7420636f6e6669673a0a0000000000213697bca95e7373787a40858a51c700000000000000000000000000000000000000000000000000000000000000ffd4bc53c27cbc5b9a9d0abce1819a619e5650a7c79c19e3f3e36cb46ad11656ef";
	const FString ExpectedHex = "0xec392c0ac580a8ce28adb765a25fabb17ccc651b7c73f8f14b2d7fb63d2f8bed";

	const TArray<uint8> RawInput = FByteArrayUtils::HexStringToBytes(Input);
	const TArray<uint8> HashedInput = FSequenceCoder::KeccakHash(RawInput);

	const FString HashedInputHex = FByteArrayUtils::BytesToHexString(HashedInput);
    
	UE_LOG(LogTemp, Display, TEXT("Keccak Hash: %s = %s"), *HashedInputHex, *ExpectedHex)
    
	TestEqual("Hex strings should match", HashedInputHex, ExpectedHex);
    
	return true;
}

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
