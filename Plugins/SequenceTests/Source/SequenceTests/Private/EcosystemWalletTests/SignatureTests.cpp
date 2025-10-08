#include "CoreMinimal.h"
#include "EcosystemWallet/Primitives/Signatures/RSY.h"
#include "Misc/AutomationTest.h"
#include "Util/ByteArrayUtils.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRSYEncoding, "SequencePlugin.UnitTests.EcosystemWallet.EncodeRSY", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FRSYEncoding::RunTest(const FString& Parameters)
{
    const FString ExpectedEncodedRsy = "0x5d0d01e6210d46ba256ac6db6750d634d263ba6ff30201c5f4ec9d1d4fe0ffa7674ef9488ab291cd3cd4fb02f64cd7367dd2735421527773b65914ecdb1d5dc7";
    
    const FString R = "42088077036100740519209836509681125922398622128241269505661854863235508928423";
    const FString S = "46727757965692218362764576115656173708633952338423148573322458993345607589319";
    constexpr int32 Y = 0;

    const FRSY RSY = FRSY(R, S, Y);

    const TArray<uint8> EncodedRsy = RSY.Pack();
    const FString RsyHex = FByteArrayUtils::BytesToHexString(EncodedRsy);

    UE_LOG(LogTemp, Log, TEXT("Encoded RSY: %s"), *RsyHex);
    
    TestEqual("Hex values should match", RsyHex, ExpectedEncodedRsy);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRSYUnpack64, "SequencePlugin.UnitTests.EcosystemWallet.RSYUnpack64", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FRSYUnpack64::RunTest(const FString& Parameters)
{
    const FString EncodedRsy = "0x5d0d01e6210d46ba256ac6db6750d634d263ba6ff30201c5f4ec9d1d4fe0ffa7674ef9488ab291cd3cd4fb02f64cd7367dd2735421527773b65914ecdb1d5dc7";
    
    const FString ExpectedR = "42088077036100740519209836509681125922398622128241269505661854863235508928423";
    const FString ExpectedS = "46727757965692218362764576115656173708633952338423148573322458993345607589319";
    constexpr int32 ExpectedY = 0;

    TArray<uint8> RsyBytes;
    if (!FByteArrayUtils::HexStringToBytes(EncodedRsy, RsyBytes))
    {
        UE_LOG(LogTemp, Log, TEXT("HexStringToBytes failed"));
        return false;
    }
    
    TSharedPtr<FRSY> RSY = FRSY::Unpack(RsyBytes);

    if (!RSY.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("Failed to unpack RSY"));
        return false;
    }

    FString ActualR = RSY.Get()->R;
    FString ActualS = RSY.Get()->S;
    int32 ActualY = RSY.Get()->YParity;
    
    TestEqual("R should match", ActualR, ExpectedR);
    TestEqual("S should match", ActualS, ExpectedS);
    TestEqual("Y should match", ActualY, ExpectedY);
    
    return true;
}
