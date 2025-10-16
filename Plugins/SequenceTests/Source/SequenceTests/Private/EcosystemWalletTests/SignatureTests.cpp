#include "CoreMinimal.h"
#include "EcosystemWallet/Primitives/Attestation/Attestation.h"
#include "EcosystemWallet/Primitives/Signatures/Erc6492.h"
#include "EcosystemWallet/Primitives/Signatures/RawSignature.h"
#include "EcosystemWallet/Primitives/Signatures/RSY.h"
#include "EcosystemWallet/Signers/SessionCredentialsSerializer.h"
#include "EthAbi/EthAbiBridge.h"
#include "Misc/AutomationTest.h"
#include "Util/ByteArrayUtils.h"
#include "Util/SequenceSupport.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRSYEncoding, "SequencePlugin.UnitTests.EcosystemWallet.EncodeRSY", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FRSYEncoding::RunTest(const FString& Parameters)
{
    const FString ExpectedEncodedRsy = "0x5d0d01e6210d46ba256ac6db6750d634d263ba6ff30201c5f4ec9d1d4fe0ffa7674ef9488ab291cd3cd4fb02f64cd7367dd2735421527773b65914ecdb1d5dc7";
    
    const FBigInt R = FBigInt("42088077036100740519209836509681125922398622128241269505661854863235508928423");
    const FBigInt S = FBigInt("46727757965692218362764576115656173708633952338423148573322458993345607589319");
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

    TArray<uint8> RsyBytes = FByteArrayUtils::HexStringToBytes(EncodedRsy);
    
    TSharedPtr<FRSY> RSY = FRSY::Unpack(RsyBytes);

    if (!RSY.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("Failed to unpack RSY"));
        return false;
    }

    FString ActualR = RSY.Get()->R.Value;
    FString ActualS = RSY.Get()->S.Value;
    int32 ActualY = RSY.Get()->YParity;
    
    TestEqual("R should match", ActualR, ExpectedR);
    TestEqual("S should match", ActualS, ExpectedS);
    TestEqual("Y should match", ActualY, ExpectedY);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCreateDeployData, "SequencePlugin.UnitTests.EcosystemWallet.CreateDeployData", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FCreateDeployData::RunTest(const FString& Parameters)
{
    const FString Stage1 = "0x00000000000084fA81809Dd337311297C5594d62";
    const FString DeployHash = "0x9a74b54e9a80f3f9df6b7bed7a71a20511566da4d49eb972f654772a1cde9236";
    const FString ExpectedHex = "0x32c02a1400000000000000000000000000000000000084fa81809dd337311297c5594d629a74b54e9a80f3f9df6b7bed7a71a20511566da4d49eb972f654772a1cde9236";

    const TArray<uint8> DeployData = FErc6492::CreateDeployData(Stage1, DeployHash);
    const FString DeployDataHex = FByteArrayUtils::BytesToHexString(DeployData);

    UE_LOG(LogTemp, Log, TEXT("DeployDataHex: %s"), *DeployDataHex);

    TestEqual("Deploy data hex should match", DeployDataHex, ExpectedHex);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDecodeErc6492, "SequencePlugin.UnitTests.EcosystemWallet.DecodeErc6492", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FDecodeErc6492::RunTest(const FString& Parameters)
{
    const FString Erc6492SignatureHex = "0x06000201d436d045ee9e004f757bb9625c97b5aca87ae374e7704cea3e4ad887e1dda22e129fef132c41ed60e026f8298f85adfdcb5ca6744856f604baf27a1ac6b8b3c90169eec95bb0ea9b3b22a355a2f21dd8b1c627b9b1a0e28f0d946f5570376295aa14cb32cfbe0ac7d8a03cafa32c62f9988f30aed37be8d2fa84881ecef80d3fb46500001511a2e70ceab3eb145f32d110383b75b330fa4e288a416062ff00003a30baef4f18c2030edbaaa55ba107059dac6f25116a79138d1af8e8126264b3ce05650000151118002fc09def9a47437cc64e270843de094f598430e289cf3a40e8417d46f15aa8a9d7973e683dee468c5048ca730aa9a9a947fd30";
    const TArray<uint8> Erc6492Signature = FByteArrayUtils::HexStringToBytes(Erc6492SignatureHex);
    
    const TTuple<TArray<uint8>, TSharedPtr<FErc6492>> Result = FErc6492::Decode(Erc6492Signature);
    const FString DecodedSignatureHex = FByteArrayUtils::BytesToHexString(Result.Key);
    const TSharedPtr<FErc6492> DecodedErc6492 = Result.Value;

    UE_LOG(LogTemp, Log, TEXT("DecodedSignatureHex: %s"), *DecodedSignatureHex);

    TestEqual("Deploy data hex should match", DecodedSignatureHex, Erc6492SignatureHex);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDecodeRawSignature, "SequencePlugin.UnitTests.EcosystemWallet.DecodeRawSignature", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FDecodeRawSignature::RunTest(const FString& Parameters)
{
    const FString ConfigUpdateSig = "0x061902118a261c1c227f653f26b468615afa440c6f70637601563941f41c2c424334601c1a283d5668dd41ebcdb83d943a8e711e02b1ae38fb5313d4cd267e4bf32701a3c332ceb8d428234ae1254b78b3c7bd871c31e050da6500004101b2ff250ab138f510fc33f7a3b32eec71b85663d1184e8d5cd59374c43d01dfaec61f3c8f83068f40dc5a5d39ee1f3e998e4477c86d3dc3e0f906eca27d7c272c416062ff00003a30bf3a81c77c768d877d9cb46558300d6bae59d96b273a5bb7bd6d0dfddb574e5b650000151118002fc09def9a47437cc64e270843de094f598430ec392c0ac580a8ce28adb765a25fabb17ccc651b7c73f8f14b2d7fb63d2f8bed";
    
    const TSharedPtr<FRawSignature> Signature = FRawSignature::Decode(FByteArrayUtils::HexStringToBytes(ConfigUpdateSig));
    const TArray<uint8> EncodedSignature = Signature->Encode();
    const FString EncodedHex = FByteArrayUtils::BytesToHexString(EncodedSignature);
    
    UE_LOG(LogTemp, Log, TEXT("Decoded RawSignature: %s = %s"), *EncodedHex, *ConfigUpdateSig);

    TestEqual("Deploy data hex should match", EncodedHex, ConfigUpdateSig);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRecoverImplicitIdentitySigner, "SequencePlugin.UnitTests.EcosystemWallet.RecoverImplicitIdentitySigner", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FRecoverImplicitIdentitySigner::RunTest(const FString& Parameters)
{
    const FString SignatureJson = "{\"type\": \"hash\",\"r\": {\"_isBigInt\": true,\"data\": \"3313507648155575330123726678207645170527620921491155233631210854395540284143\"},\"s\": {\"_isBigInt\": true,\"data\": \"12599654210633038267252651539192195082231863727699109387705650514161424877718\"},\"yParity\": 1}";
    const FString AttestationJson = "{\"approvedSigner\":\"0x3761e16647E27956a5BfAdFE507d4c4F9a6f5512\",\"identityType\":{\"_isUint8Array\":true,\"data\":\"0x00000002\"},\"issuerHash\":{\"_isUint8Array\":true,\"data\":\"0x4dcc430b541f16ee48b99ac8df13c9f8fa820c59de2e8bfc834cd295504d50dc\"},\"audienceHash\":{\"_isUint8Array\":true,\"data\":\"0x33f23bc388d939c925129d46379b8a32475568969cb4d9e9f918c9dff965e700\"},\"applicationData\":{\"_isUint8Array\":true,\"data\":\"0x\"},\"authData\":{\"redirectUrl\":\"http://localhost:4444\",\"issuedAt\":{\"_isBigInt\":true,\"data\":\"1760602234\"}}}";
    const FString ExpectedAddress = "0x8A261C1C227F653f26b468615Afa440c6f706376";

    FRSY Signature;
    const bool SignatureResult = FSessionCredentialsSerializer::ParseSignature(USequenceSupport::JsonStringToObject(SignatureJson), Signature);

    FAttestation Attestation;
    const bool AttestationResult = FSessionCredentialsSerializer::ParseAttestation(USequenceSupport::JsonStringToObject(AttestationJson), Attestation);

    if (!SignatureResult || !AttestationResult)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse signature and/or attestation"));
        return false;
    }

    TArray<uint8> OutPublicKey;
    TArray<uint8> OutAddress;

    UE_LOG(LogTemp, Log, TEXT("Sig pack65 %s Attestation %s"), *FByteArrayUtils::BytesToHexString(Signature.Pack65()), *FByteArrayUtils::BytesToHexString(Attestation.Hash()));
    
    const bool Result = USequenceSupport::RecoverEthPubAndAddress(
        Signature.Pack65(),
        Attestation.Hash(),
        OutPublicKey,
        OutAddress);
    
    if (!Result)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to recover signature"));
        return false;
    }

    const FString PublicKeyHex = FByteArrayUtils::BytesToHexString(OutPublicKey);
    const FString AddressHex = FByteArrayUtils::BytesToHexString(OutAddress);

    UE_LOG(LogTemp, Display, TEXT("Recover Implicit Identity Signer %s %s"), *PublicKeyHex, *AddressHex);
    
    TestEqual("Deploy data hex should match", AddressHex, ExpectedAddress);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAttestationHash, "SequencePlugin.UnitTests.EcosystemWallet.AttestationHash", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FAttestationHash::RunTest(const FString& Parameters)
{
    const FString AttestationJson = "{\"approvedSigner\":\"0xBc68eef2E97d7eBAa528EA2597F9a9bbEaa31013\",\"identityType\":{\"_isUint8Array\":true,\"data\":\"0x00000002\"},\"issuerHash\":{\"_isUint8Array\":true,\"data\":\"0x4dcc430b541f16ee48b99ac8df13c9f8fa820c59de2e8bfc834cd295504d50dc\"},\"audienceHash\":{\"_isUint8Array\":true,\"data\":\"0x33f23bc388d939c925129d46379b8a32475568969cb4d9e9f918c9dff965e700\"},\"applicationData\":{\"_isUint8Array\":true,\"data\":\"0x\"},\"authData\":{\"redirectUrl\":\"http://localhost:4444\",\"issuedAt\":{\"_isBigInt\":true,\"data\":\"1760598705\"}}}";
    const FString ExpectedHash = "0xd7d858321f87004bd3761ae41aab0762c3f67946a3e2d6892f364aa444484993";

    FAttestation Attestation;
    const bool Result = FSessionCredentialsSerializer::ParseAttestation(USequenceSupport::JsonStringToObject(AttestationJson), Attestation);
    
    if (!Result)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to recover signature"));
        return false;
    }

    const FString AttestationHash = FByteArrayUtils::BytesToHexString(Attestation.Hash());

    UE_LOG(LogTemp, Display, TEXT("Hashing attestation %s %s"), *AttestationHash, *ExpectedHash);
    
    TestEqual("Attestation hash should match", AttestationHash, ExpectedHash);
    
    return true;
}
