// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "ABI/ABI.h"
#include "Misc/AutomationTest.h"
#include "Types/BinaryData.h"


IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestABI, "Public.Tests.TestABI",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestABI::RunTest(const FString& Parameters)
{
	// This example encodes a function call to a function with signature g(uint256[][],string[])
	// and values ([[1, 2], [3]], ["one", "two", "three"])

	TFixedABIData Int1 = ABI::UInt32(1);
	TFixedABIData Int2 = ABI::UInt32(2);
	TDynamicABIArray IntArray1;
	IntArray1.Push(MakeShared<TFixedABIData>(Int1));
	IntArray1.Push(MakeShared<TFixedABIData>(Int2));

	TFixedABIData Int3 = ABI::UInt32(3);
	TDynamicABIArray IntArray2;
	IntArray2.Push(MakeShared<TFixedABIData>(Int3));
	
	TDynamicABIArray IntArrayArray;
	IntArrayArray.Push(MakeShared<TDynamicABIArray>(IntArray1));
	IntArrayArray.Push(MakeShared<TDynamicABIArray>(IntArray2));
	
	TDynamicABIData String1 = ABI::String("one");
	TDynamicABIData String2 = ABI::String("two");
	TDynamicABIData String3 = ABI::String("three");
	TDynamicABIArray StringArray;
	StringArray.Push(MakeShared<TDynamicABIData>(String1));
	StringArray.Push(MakeShared<TDynamicABIData>(String2));
	StringArray.Push(MakeShared<TDynamicABIData>(String3));

	// We push all the arguments in an array
	TArray<TSharedPtr<ABIElement>> Arr = TArray<TSharedPtr<ABIElement>>();
	Arr.Push(MakeShared<TDynamicABIArray>(IntArrayArray));
	Arr.Push(MakeShared<TDynamicABIArray>(StringArray));
	
	UE_LOG(LogTemp, Display, TEXT("%s"), *ABI::Display("g(uint256[][],string[])", Arr));

	FUnsizedData Encoded = ABI::Encode("g(uint256[][],string[])", Arr);

	FString Correct = "2289b18c"
					"0000000000000000000000000000000000000000000000000000000000000040"
					"0000000000000000000000000000000000000000000000000000000000000140"
					"0000000000000000000000000000000000000000000000000000000000000002"
					"0000000000000000000000000000000000000000000000000000000000000040"
					"00000000000000000000000000000000000000000000000000000000000000a0"
					"0000000000000000000000000000000000000000000000000000000000000002"
					"0000000000000000000000000000000000000000000000000000000000000001"
					"0000000000000000000000000000000000000000000000000000000000000002"
					"0000000000000000000000000000000000000000000000000000000000000001"
					"0000000000000000000000000000000000000000000000000000000000000003"
					"0000000000000000000000000000000000000000000000000000000000000003"
					"0000000000000000000000000000000000000000000000000000000000000060"
					"00000000000000000000000000000000000000000000000000000000000000a0"
					"00000000000000000000000000000000000000000000000000000000000000e0"
					"0000000000000000000000000000000000000000000000000000000000000003"
					"6f6e650000000000000000000000000000000000000000000000000000000000"
					"0000000000000000000000000000000000000000000000000000000000000003"
					"74776f0000000000000000000000000000000000000000000000000000000000"
					"0000000000000000000000000000000000000000000000000000000000000005"
					"7468726565000000000000000000000000000000000000000000000000000000";
	
	if(Correct != Encoded.ToHex())
	{
		return false;
	}
	
	TSharedPtr<TDynamicABIArray> DecodeIntArray = MakeShared<TDynamicABIArray>(TDynamicABIArray());
	DecodeIntArray->Push(MakeShared<TFixedABIData>(ABI::UInt32()));
	
	TSharedPtr<TDynamicABIArray> DecodeIntArrayArray = MakeShared<TDynamicABIArray>(TDynamicABIArray());
	DecodeIntArrayArray->Push(DecodeIntArray);
	
	TSharedPtr<TDynamicABIArray> DecodeStringArray = MakeShared<TDynamicABIArray>(TDynamicABIArray());
	DecodeStringArray->Push(MakeShared<TDynamicABIData>(ABI::String()));

	ABI::Decode(*Encoded.Arr, TArray<TSharedPtr<ABIElement>>{DecodeIntArrayArray, DecodeStringArray});

	// Loop through array of integer arrays
	for(auto Element : DecodeIntArrayArray->AsArray())
	{
		// Loop through integer array
		for(auto SubElement : Element->AsArray())
		{
			UE_LOG(LogTemp, Display, TEXT("%d"), SubElement->AsInt32());
		}
	}

	// Loop through string array
	for(auto Element : DecodeStringArray->AsArray())
	{
		UE_LOG(LogTemp, Display, TEXT("%s"), *Element->AsString());
	}

	if(DecodeIntArrayArray->AsArray()[0]->AsArray()[0]->AsInt32() != 1) return false;
	if(DecodeIntArrayArray->AsArray()[0]->AsArray()[1]->AsInt32() != 2) return false;
	if(DecodeIntArrayArray->AsArray()[1]->AsArray()[0]->AsInt32() != 3) return false;
	if(DecodeStringArray->AsArray()[0]->AsString() != "one") return false;
	if(DecodeStringArray->AsArray()[1]->AsString() != "two") return false;
	if(DecodeStringArray->AsArray()[2]->AsString() != "three") return false;
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}

