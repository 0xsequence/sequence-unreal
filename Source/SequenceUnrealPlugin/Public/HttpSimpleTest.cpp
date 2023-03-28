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

	LogIfError(provider.BlockByNumber(1));
	LogIfError(provider.ChainId());
	LogIfError(provider.BlockNumber());
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
