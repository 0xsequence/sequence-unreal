#include "HexUtility.h"
#include "Misc/AutomationTest.h"
#include "Provider.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Ecdsa.hpp"
#include "secp256k1Library/secp256k1/secp256k1.h"
#include "Crypto.h"

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
	Hash256 hash = HexStringToHash256("0x5d46db9687f11dcf56122558a6aca31d91607d63c5df9cbc417c842913df000a");
	Address addr = HexStringToAddress("0xc683a014955b75F5ECF991d4502427c8fa1Aa249");

	if(false)
	{
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
	}

	auto PrivateKey = HexStringToHash256("0xabc0000000000000000000000000000000000000000000000000000000000001");
	Hash256 PublicKey = GetPublicKey(PrivateKey);
	//auto PublicKey = HexStringToHash(GPublicKeyByteLength, "");
	//Keccak256::getHash(PrivateKey, 32, PublicKey);

	//FString h = Hash256ToHexString(PublicKey);
	UE_LOG(LogTemp, Display, TEXT("RESULT: %s"), *HashToHexString(GPublicKeyByteLength, PublicKey));

	//Keccak256::getHash("\x19Ethereum Signed Message:\n32" + Keccak256::getHash(message))

	//Ecdsa::sign(PrivateKey, )
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
