#include "Misc/AutomationTest.h"
#include "Provider.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(HttpSimpleTest, "Public.HttpSimpleTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool HttpSimpleTest::RunTest(const FString& Parameters)
{
	auto provider = Provider("http://localhost:8545/");

	auto res = provider.BlockByNumber(1);

	UE_LOG(LogTemp, Display, TEXT("block by num: %s"), *res);

	auto chainId = provider.ChainId();

	if(chainId.HasValue())
	{
		UE_LOG(LogTemp, Display, TEXT("chain id: %s"), *FString::FromInt(chainId.GetValue()));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("chain id: %s"), *chainId.GetError().Message);
	}
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
