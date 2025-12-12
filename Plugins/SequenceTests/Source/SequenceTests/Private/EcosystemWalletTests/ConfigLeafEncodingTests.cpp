#include "CoreMinimal.h"
#include "EcosystemWallet/Primitives/Calls/Calls.h"
#include "EcosystemWallet/Primitives/Config/Leafs/ConfigSapientSignerLeaf.h"
#include "EcosystemWallet/Primitives/Config/Leafs/ConfigSignerLeaf.h"
#include "EcosystemWallet/Primitives/Signatures/SignatureOfSapientSignerLeaf.h"
#include "EcosystemWallet/Primitives/Signatures/SignatureOfSignerLeafHash.h"
#include "Misc/AutomationTest.h"
#include "Util/ByteArrayUtils.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEncodeSapientSigner, "SequencePlugin.UnitTests.EcosystemWallet.ConfigLeafEncoding.EncodeSapientSigner", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FEncodeSapientSigner::RunTest(const FString& Parameters)
{
    const FString Address = "0x0000000000213697bCA95E7373787a40858a51C7";
    const FBigInt Weight = FBigInt("255");
    const FString ImageHash = "0xd4bc53c27cbc5b9a9d0abce1819a619e5650a7c79c19e3f3e36cb46ad11656ef";

    FConfigSapientSignerLeaf SapientSignerLeaf = FConfigSapientSignerLeaf(Address, Weight.Value, ImageHash);
    const FString EncodedLeaf = FByteArrayUtils::BytesToHexString(SapientSignerLeaf.Encode(false, TArray<uint8>()));
    
    const FString ExpectedHex = "0x30ec392c0ac580a8ce28adb765a25fabb17ccc651b7c73f8f14b2d7fb63d2f8bed";
    
    UE_LOG(LogTemp, Log, TEXT("Encoded leaf: %s == %s"), *EncodedLeaf, *ExpectedHex);
    
    TestEqual("Hex values should match", EncodedLeaf, ExpectedHex);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEncodeSigner, "SequencePlugin.UnitTests.EcosystemWallet.ConfigLeafEncoding.EncodeSigner", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FEncodeSigner::RunTest(const FString& Parameters)
{
    const FString Address = "0x8a261c1c227f653f26b468615afa440c6f706376";
    const FBigInt Weight = FBigInt("1");

    FConfigSignerLeaf SapientSignerLeaf = FConfigSignerLeaf(Address, Weight.Value);
    const FString EncodedLeaf = FByteArrayUtils::BytesToHexString(SapientSignerLeaf.Encode(false, TArray<uint8>()));
    
    const FString ExpectedHex = "0x118a261c1c227f653f26b468615afa440c6f706376";
    
    UE_LOG(LogTemp, Log, TEXT("Encoded leaf: %s == %s"), *EncodedLeaf, *ExpectedHex);
    
    TestEqual("Hex values should match", EncodedLeaf, ExpectedHex);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEncodeSapientSignature, "SequencePlugin.UnitTests.EcosystemWallet.ConfigLeafEncoding.EncodeSapientSignature", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FEncodeSapientSignature::RunTest(const FString& Parameters)
{
    const FString Address = "0x0000000000CC58810c33F3a0D78aA1Ed80FaDcD8";
    const FString Data = "0x0001ea2201e721c2219f217c21591069c322e3248a5dfc29d73c5b0553b0185a35cd5bb6386747517ef7e53b15e2872136408a261c1c227f653f26b468615afa440c6f7063761097e0c00a3fd6c4acf228d1f021759ff488f78d679bd7fd0eb00f4ccb9afec5ac1046424c6104f1e436a2a48c56019c471b13e8111b7996b9c00581b780b86b897e1053d04b3c489d66449c1dcae8bfaba8724753fc0ef121983b2e55e904b8daaeca1031f93c9ef5995e2b3a67fddf148fd78b1cfd19a225c9f3dc8c016d09ec9e5143220120106185e852b487e57c5b8cf071455645489401a180038125f5b3937c0e9ae230bf21fd10626449635a9fd1b159d741a5e314499a1519cba2d38bc8cea48c12d49571292721da10892d023bb44af2498b617b45155cd289fa3e8e75d7216923160b5d1dc9f1f22f21b710ccbfa6402d8830ab946a25469b1d1613caf0b83bc620aa0780acb2399fb3d40c219400ece4b7cc5f61a3de9930b185e20bda60b90847240000000000000000000000000000000000000000000000000000000000066eee000000000000000000000000000000000000000000000000000000000000000000000199e2bf9b400133985d320809e26274a72e03268c8a29927bc6da0010a6884d80bbae99b03bbb09195b87c6503c0922313908148008475f24e96f8e1b000077c0a07eefeeb150358ea011cefd71ebb462a0fd088201d277541b12c0fad5f076c9a655a648df2e6fbc40c6bf195b9b5c24e8d37ac3fcdd3bf962d03d840cc6";

    FSignatureOfSapientSignerLeaf SapientSignerLeaf = FSignatureOfSapientSignerLeaf(ESapientSignatureType::SapientSignature, Address, FByteArrayUtils::HexStringToBytes(Data));
    const FString EncodedLeaf = FByteArrayUtils::BytesToHexString(SapientSignerLeaf.Encode("1"));
    
    const FString ExpectedHex = "0x990000000000cc58810c33f3a0d78aa1ed80fadcd8022f0001ea2201e721c2219f217c21591069c322e3248a5dfc29d73c5b0553b0185a35cd5bb6386747517ef7e53b15e2872136408a261c1c227f653f26b468615afa440c6f7063761097e0c00a3fd6c4acf228d1f021759ff488f78d679bd7fd0eb00f4ccb9afec5ac1046424c6104f1e436a2a48c56019c471b13e8111b7996b9c00581b780b86b897e1053d04b3c489d66449c1dcae8bfaba8724753fc0ef121983b2e55e904b8daaeca1031f93c9ef5995e2b3a67fddf148fd78b1cfd19a225c9f3dc8c016d09ec9e5143220120106185e852b487e57c5b8cf071455645489401a180038125f5b3937c0e9ae230bf21fd10626449635a9fd1b159d741a5e314499a1519cba2d38bc8cea48c12d49571292721da10892d023bb44af2498b617b45155cd289fa3e8e75d7216923160b5d1dc9f1f22f21b710ccbfa6402d8830ab946a25469b1d1613caf0b83bc620aa0780acb2399fb3d40c219400ece4b7cc5f61a3de9930b185e20bda60b90847240000000000000000000000000000000000000000000000000000000000066eee000000000000000000000000000000000000000000000000000000000000000000000199e2bf9b400133985d320809e26274a72e03268c8a29927bc6da0010a6884d80bbae99b03bbb09195b87c6503c0922313908148008475f24e96f8e1b000077c0a07eefeeb150358ea011cefd71ebb462a0fd088201d277541b12c0fad5f076c9a655a648df2e6fbc40c6bf195b9b5c24e8d37ac3fcdd3bf962d03d840cc6";
    
    UE_LOG(LogTemp, Log, TEXT("Encoded leaf: %s == %s"), *EncodedLeaf, *ExpectedHex);
    
    TestEqual("Hex values should match", EncodedLeaf, ExpectedHex);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEncodeHashSignature, "SequencePlugin.UnitTests.EcosystemWallet.ConfigLeafEncoding.EncodeHashSignature", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FEncodeHashSignature::RunTest(const FString& Parameters)
{
    const FBigInt R = FBigInt("2433345314921777822641617144758535808198909158582539987905218152612157331551");
    const FBigInt S = FBigInt("1114976271722566865069336232019550882276292950343993705913353163478553923110");
    constexpr uint32 Y = 1;
    
    const TSharedPtr<FRSY> Signature = MakeShared<FRSY>(FRSY(R, S, Y));
    FSignatureOfSignerLeafHash SignatureOfLeaf = FSignatureOfSignerLeafHash();
    SignatureOfLeaf.Signature = Signature;
    
    const FString EncodedLeaf = FByteArrayUtils::BytesToHexString(SignatureOfLeaf.Encode("1"));
    
    const FString ExpectedHex = "0x010561398216aabd75798fc5c816197228bbc044c8f008472fbe5545e018da185f82770de0730e6aed7dfcf9fec27c3c9f42c937009954184c8eba813bf21d0e26";
    
    UE_LOG(LogTemp, Log, TEXT("Encoded leaf: %s == %s"), *EncodedLeaf, *ExpectedHex);
    
    TestEqual("Hex values should match", EncodedLeaf, ExpectedHex);
    
    return true;
}
