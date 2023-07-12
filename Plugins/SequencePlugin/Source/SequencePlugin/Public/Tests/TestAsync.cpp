#include "Misc/AutomationTest.h"
#include "Async.h"

void Hello()
{
	UE_LOG(LogTemp, Display, TEXT("Hello world"));
}

void Print(FString Input)
{
	UE_LOG(LogTemp, Display, TEXT("I am printing %s"), *Input);
}

FString LateHello()
{
	FPlatformProcess::Sleep(3.0f);

	return "Hello World";
}

class MyClass
{
public:
	void MyPrint(FString Input)
	{
		UE_LOG(LogTemp, Display, TEXT("MyClass is printing %s"), *Input);
	}
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestAsync, "Public.Tests.TestAsync",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestAsync::RunTest(const FString& Parameters)
{
	Async(EAsyncExecution::Thread, []
	{
		// code goes here
		auto Data = Async(EAsyncExecution::Thread, LateHello);
		Print(Data.Get());
	}).Wait();

	MyClass MyObject = MyClass();

	auto MyFuture = Async(EAsyncExecution::Thread, LateHello);
	TFunction<void (FString)> Func = [&MyObject](FString Input){MyObject.MyPrint(Input);};
	SendAsync(MyFuture, Func).Wait();

	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
