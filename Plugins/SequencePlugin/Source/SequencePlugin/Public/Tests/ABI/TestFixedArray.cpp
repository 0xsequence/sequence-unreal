#include "Misc/AutomationTest.h"
#include "HexUtility.h"
#include "ABI/ABI.h"
#include "ABI/ABITypes.h"
#include "ABI/ABIDynamicArray.h"
#include "ABI/ABIFixedArray.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestFixedArray, "Public.Tests.ABI.TestFixedArray",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestFixedArray::RunTest(const FString& Parameters)
{
	uint32 Number1 = 1;
	auto NumberProp1 = FABIUInt32Property(Number1);

	uint32 Number2 = 2;
	auto NumberProp2 = FABIUInt32Property(Number2);

	uint32 Number3 = 3;
	auto NumberProp3 = FABIUInt32Property(Number3);

	FString String1 = "one";
	auto StringProp1 = FABIStringProperty(String1);
	
	FString String2 = "two";
	auto StringProp2 = FABIStringProperty(String2);
	
	FString String3 = "three";
	auto StringProp3 = FABIStringProperty(String3);

	auto ArrayProp1 = FABIDynamicArrayProperty<FABIUInt32Property>(new TArray{NumberProp1, NumberProp2});
	auto ArrayProp2 = FABIDynamicArrayProperty<FABIUInt32Property>(new TArray{NumberProp3});
	auto ArrayProp3 = FABIFixedArrayProperty<FABIDynamicArrayProperty<FABIUInt32Property>, 2>(new FABIDynamicArrayProperty<FABIUInt32Property>[]{ArrayProp1, ArrayProp2});
	auto ArrayProp4 = FABIFixedArrayProperty<FABIStringProperty, 3>(new FABIStringProperty[]{StringProp1, StringProp2, StringProp3});
	
	TArray<FABIProperty*> Properties;
    Properties.Push(&ArrayProp3);
    Properties.Push(&ArrayProp4);
    auto Obj = ABI::Encode("newFun(string,address)", Properties);
	uint32 BlockNum = (Obj.Length - GMethodIdByteLength) / GBlockByteLength;
	
	UE_LOG(LogTemp, Display, TEXT("The length is %i"), BlockNum);

	if(true)
	{
		UE_LOG(LogTemp, Display, TEXT("HEADER: %s"), *HashToHexString(GMethodIdByteLength, &Obj.Arr[0]));
		for(auto i = 0; i < BlockNum; i++)
		{
			auto Addr = GMethodIdByteLength + GBlockByteLength * i;
			UE_LOG(LogTemp, Display, TEXT("%i %s"), Addr, *HashToHexString(GBlockByteLength, &Obj.Arr[Addr]));
		}
	}
	
	TArray<FABIProperty*> DecodeProperties;
	auto Arr1 = FABIFixedArrayProperty<FABIDynamicArrayProperty<FABIUInt32Property>, 2>{};
	auto Arr2 = FABIFixedArrayProperty<FABIStringProperty, 3>{};
	DecodeProperties.Push(&Arr1);
	DecodeProperties.Push(&Arr2);
	ABI::Decode(Obj, DecodeProperties);

	for(auto i = 0; i < 2; i++)
	{
		auto Prop = Arr1.GetValue()[i];
		for(auto InnerProp : *Prop.GetValue())
		{
			UE_LOG(LogTemp, Display, TEXT("The Value of the int is %i"), InnerProp.GetValue());
		}
	}

	for(auto i = 0; i < 3; i++)
	{
		auto Prop = Arr2.GetValue()[i];
		UE_LOG(LogTemp, Display, TEXT("The Value of the string is %s"), *Prop.GetValue());
	}
	
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
