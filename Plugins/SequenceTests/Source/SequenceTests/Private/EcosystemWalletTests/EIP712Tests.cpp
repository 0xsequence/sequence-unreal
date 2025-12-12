#include "CoreMinimal.h"
#include "EcosystemWallet/Primitives/Calls/Calls.h"
#include "EcosystemWallet/Primitives/Calls/CallTypedData.h"
#include "EcosystemWallet/Primitives/Calls/CallTypedDataFactory.h"
#include "Misc/AutomationTest.h"
#include "Util/ByteArrayUtils.h"
#include "Util/SequenceSupport.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTypedData, "SequencePlugin.UnitTests.EcosystemWallet.EIP712Tests.TypedData", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FTypedData::RunTest(const FString& Parameters)
{
    const FString DomainJson = "{\"domain\":{\"name\":\"Sequence Wallet\",\"version\":\"3\",\"chainId\":421614,\"verifyingContract\":\"0xbd7f38b943452e0c14d7ba92b9b504a9c9fc3518\"},\"types\":{\"Calls\":[{\"name\":\"calls\",\"type\":\"Call[]\"},{\"name\":\"space\",\"type\":\"uint256\"},{\"name\":\"nonce\",\"type\":\"uint256\"},{\"name\":\"wallets\",\"type\":\"address[]\"}],\"Call\":[{\"name\":\"to\",\"type\":\"address\"},{\"name\":\"value\",\"type\":\"uint256\"},{\"name\":\"data\",\"type\":\"bytes\"},{\"name\":\"gasLimit\",\"type\":\"uint256\"},{\"name\":\"delegateCall\",\"type\":\"bool\"},{\"name\":\"onlyFallback\",\"type\":\"bool\"},{\"name\":\"behaviorOnError\",\"type\":\"uint256\"}]},\"primaryType\":\"Calls\",\"message\":{\"space\":\"0\",\"nonce\":\"34\",\"wallets\":[],\"calls\":[{\"gasLimit\":0,\"delegateCall\":false,\"data\":\"0x0c0340fc\",\"onlyFallback\":false,\"to\":\"0x33985d320809E26274a72E03268c8a29927Bc6dA\",\"value\":0,\"behaviorOnError\":1}]}}";
    const FString ExpectedEncodedHash = "0xcf6b4c5e474e071e39038bd2a3f39dd8612593ced95c5dad09dc1a193c0d5ac9";

    const TArray<uint8> HashedData = USequenceSupport::EncodeAndHashTypedData(DomainJson);
    const FString HashedHex = FByteArrayUtils::BytesToHexString(HashedData);
    
    UE_LOG(LogTemp, Log, TEXT("Encoded EIP712: %s == %s"), *HashedHex, *ExpectedEncodedHash);
    
    TestEqual("Hex values should match", HashedHex, ExpectedEncodedHash);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTypedDataFromCalls, "SequencePlugin.UnitTests.EcosystemWallet.EIP712Tests.TypedDataFromCalls", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FTypedDataFromCalls::RunTest(const FString& Parameters)
{
    const FString ExpectedEncodedHash = "0xcf6b4c5e474e071e39038bd2a3f39dd8612593ced95c5dad09dc1a193c0d5ac9";
    
    FCalls Calls;
    Calls.Nonce = FBigInt("34");
    Calls.Space = FBigInt("0");
    Calls.Calls.Add(FCall {
        "0x33985d320809E26274a72E03268c8a29927Bc6dA",
        "0x0c0340fc",
        FBigInt("0"),
        FBigInt("0"),
        false,
        false,
        "1"
    });

    TSharedPtr<FCalls> CallsPtr = MakeShared<FCalls>(Calls);
    FString DomainJson = FCallTypedDataFactory::FromCalls("0xbd7f38b943452e0c14d7ba92b9b504a9c9fc3518", FBigInt("421614"), CallsPtr);
    DomainJson = DomainJson.Replace(TEXT("\n"), TEXT(""));
    DomainJson = DomainJson.Replace(TEXT("\t"), TEXT(""));

    UE_LOG(LogTemp, Log, TEXT("EIP712 Domain Json: %s"), *DomainJson);

    const TArray<uint8> HashedData = USequenceSupport::EncodeAndHashTypedData(DomainJson);
    const FString HashedHex = FByteArrayUtils::BytesToHexString(HashedData);
    
    UE_LOG(LogTemp, Log, TEXT("Encoded EIP712: %s == %s"), *HashedHex, *ExpectedEncodedHash);
    
    TestEqual("Hex values should match", HashedHex, ExpectedEncodedHash);
    
    return true;
}
