// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "ABI/ABI.h"
#include "Misc/AutomationTest.h"
#include "Types/BinaryData.h"


IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestABI, "Public.Tests.TestABI",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestABI::RunTest(const FString& Parameters)
{

	TFixedABIData Int1 = ABI::UInt32(1);
	TFixedABIData Int2 = ABI::UInt32(2);
	TDynamicABIArray IntArray1;
	IntArray1.Push(&Int1);
	IntArray1.Push(&Int2);

	TFixedABIData Int3 = ABI::UInt32(3);
	TDynamicABIArray IntArray2;
	IntArray2.Push(&Int3);
	
	TDynamicABIArray IntArrayArray;
	IntArrayArray.Push(&IntArray1);
	IntArrayArray.Push(&IntArray2);
	
	TDynamicABIData String1 = ABI::String("one");
	TDynamicABIData String2 = ABI::String("two");
	TDynamicABIData String3 = ABI::String("three");
	TDynamicABIArray StringArray;
	StringArray.Push(&String1);
	StringArray.Push(&String2);
	StringArray.Push(&String3);
	
	TArray<ABIEncodeable*> Arr = TArray<ABIEncodeable*>();
	Arr.Push(&IntArrayArray);
	Arr.Push(&StringArray);
	
	UE_LOG(LogTemp, Display, TEXT("%s"), *ABI::Display("g(uint256[][],string[])", Arr));
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}

