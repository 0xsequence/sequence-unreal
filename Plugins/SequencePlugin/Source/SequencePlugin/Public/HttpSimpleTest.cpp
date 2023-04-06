#include "HexUtility.h"
#include "Misc/AutomationTest.h"
#include "Provider.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(HttpSimpleTest, "Public.HttpSimpleTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

template<typename T>
void LogIfError(TResult<T> Result)
{
	if(!Result.HasValue())
	{
		UE_LOG(LogTemp, Error, TEXT("Error: %s"), *Result.GetError().Message);
	}
}

bool HttpSimpleTest::RunTest(const FString& Parameters)
{
	auto provider = Provider("http://localhost:8545/");
	Hash256 hash = HexStringToHash256("0x5d46db9687f11dcf56122558a6aca31d91607d63c5df9cbc417c842913df000a");
	Address addr = HexStringToAddress("0xB5c3023dbEcE7a6Bb78014000CD1C8ce940B50a0");

	LogIfError(provider.BlockByNumber(1));
	LogIfError(provider.BlockByNumber(Latest));
	LogIfError(provider.ChainId());
	LogIfError(provider.BlockNumber());
	LogIfError(provider.BlockByHash(hash));
	//LogIfError(provider.TransactionCount(addr, 1));
	//LogIfError(provider.TransactionCount(addr, Latest));

	delete[] hash;
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
