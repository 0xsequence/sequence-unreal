#include <cstdlib>

#include "HexUtility.h"
#include "Misc/AutomationTest.h"
#include "Provider.h"
#include "secp256k1Library/secp256k1/secp256k1.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(HttpSimpleTest, "Public.HttpSimpleTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

template<typename T>
void LogIfError(FString Name, TResult<T> Result)
{
	UE_LOG(LogTemp, Display, TEXT("RUNNING: %s"), *Name);
	if(!Result.HasValue())
	{
		UE_LOG(LogTemp, Error, TEXT(" ERROR: %s"), *Result.GetError().Message);
	}
}

bool HttpSimpleTest::RunTest(const FString& Parameters)
{
	auto provider = Provider("http://localhost:8545/");
	Hash256 hash = HexStringToHash256("0x5d46db9687f11dcf56122558a6aca31d91607d63c5df9cbc417c842913df000a");
	Address addr = HexStringToAddress("0xB5c3023dbEcE7a6Bb78014000CD1C8ce940B50a0");

	LogIfError("BlockByNumber", provider.BlockByNumber(1));
	LogIfError("BlockByNumber", provider.BlockByNumber(Latest));
	LogIfError("ChainID", provider.ChainId());
	LogIfError("BlockNumber", provider.BlockNumber());
	//LogIfError("BlockByHash", provider.BlockByHash(hash));
	//LogIfError(provider.TransactionCount(addr, 1));
	//LogIfError(provider.TransactionCount(addr, Latest));
	LogIfError("HeaderByNumber", provider.HeaderByNumber(1));
	LogIfError("HeaderByNumber", provider.HeaderByNumber(Latest));

	uint8_t x = 5;
	Hash256 y = new uint8_t[32];

	Keccak256::getHash(&x, 1, y);

	FString h = Hash256ToHexString(y);
	UE_LOG(LogTemp, Display, TEXT("RESULT: %s"), *h);
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
