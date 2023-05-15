#include "HexUtility.h"
#include "Misc/AutomationTest.h"
#include "Provider.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Ecdsa.hpp"
#include "Crypto.h"
#include "RLP.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(HttpSimpleTest, "Public.HttpSimpleTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

template<typename T>
void LogIfError(FString Name, TResult<T> Result)
{
	if(!Result.HasValue())
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR for %s: %s"), *Name, *Result.GetError().Message);
	}
}

bool HttpSimpleTest::RunTest(const FString& Parameters)
{
	auto provider = Provider("http://localhost:8545/");
	auto hash = FHash256::From(HexStringToBinary("0xabc0000000000000000000000000000000000000000000000000000000000001").Arr);
	auto addr = FAddress::From(HexStringToBinary("0x1099542D7dFaF6757527146C0aB9E70A967f71C0").Arr);

	LogIfError("BlockByNumber", provider.BlockByNumber(1));
	LogIfError("BlockByNumber", provider.BlockByNumber(Latest));
	LogIfError("ChainID", provider.ChainId());
	LogIfError("BlockNumber", provider.BlockNumber());
	//LogIfError("BlockByHash", provider.BlockByHash(hash));
	//LogIfError(provider.TransactionCount(addr, 1));
	//LogIfError(provider.TransactionCount(addr, Latest));
	LogIfError("HeaderByNumber", provider.HeaderByNumber(1));
	LogIfError("HeaderByNumber", provider.HeaderByNumber(Latest));
	LogIfError("NonceAt", provider.NonceAt(1));
	LogIfError("NonceAt", provider.NonceAt(Latest));

	auto PrivateKey = FPrivateKey::From(HexStringToBinary("0xabc0000000000000000000000000000000000000000000000000000000000001").Arr);
	FPublicKey PublicKey = GetPublicKey(PrivateKey);
	//auto PublicKey = HexStringToHash(GPublicKeyByteLength, "");
	//Keccak256::getHash(PrivateKey, 32, PublicKey);

	auto item = Itemize(new RLPItem[]
	{
		Itemize(HexStringToBinary("0x09")), // Nonce
		Itemize(HexStringToBinary("0x4A817C800")), // GasPrice
		Itemize(HexStringToBinary("0x5208")), // GasLimit
		Itemize(addr), // To
		Itemize(HexStringToBinary("0xDE0B6B3A7640000")), // Value
		Itemize(HexStringToBinary("")), // Data
		Itemize(HexStringToBinary("0x1")), // V
		Itemize(HexStringToBinary("0x")), // R 
		Itemize(HexStringToBinary("0x")), // S
	}, 9);
	auto EncodedSigningData = RLP::Encode(item);
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}