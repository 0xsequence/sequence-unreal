#pragma warning(disable: 4018)
#include "Util/HexUtility.h"
#include "ABI/ABI.h"
#include "Misc/AutomationTest.h"
#include "ABI/ABITypes.h"
#include "ABI/ABIDynamicArray.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestNewABI, "Public.Tests.TestNewABI",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestNewABI::RunTest(const FString& Parameters)
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
	auto ArrayProp3 = FABIDynamicArrayProperty<FABIDynamicArrayProperty<FABIUInt32Property>>(new TArray{ArrayProp1, ArrayProp2});
	auto ArrayProp4 = FABIDynamicArrayProperty<FABIStringProperty>(new TArray{StringProp1, StringProp2, StringProp3});
	
	TArray<FABIProperty*> Properties;
    Properties.Push(&ArrayProp3);
    Properties.Push(&ArrayProp4);
    auto Obj = ABI::Encode("newFun(string,address)", Properties);
	uint32 BlockNum = (Obj.Length - GMethodIdByteLength) / GBlockByteLength;
	
	UE_LOG(LogTemp, Display, TEXT("The length is %i"), BlockNum);

	if(true)
	{
		//UE_LOG(LogTemp, Display, TEXT("HEADER: %s"), *HashToHexString(GMethodIdByteLength, &Obj.Arr[0]));
		UE_LOG(LogTemp, Display, TEXT("HEADER: %s"), *BytesToHex(&Obj.Arr[0], GMethodIdByteLength));
		for(auto i = 0; i < BlockNum; i++)
		{
			auto Addr = GMethodIdByteLength + GBlockByteLength * i;
			//UE_LOG(LogTemp, Display, TEXT("%i %s"), Addr, *HashToHexString(GBlockByteLength, &Obj.Arr[Addr]));
			UE_LOG(LogTemp, Display, TEXT("%i %s"), Addr, *BytesToHex(&Obj.Arr[Addr], GBlockByteLength));
		}
	}

	TArray<FABIProperty*> DecodeProperties;
	auto Arr1 = FABIDynamicArrayProperty<FABIDynamicArrayProperty<FABIUInt32Property>>{};
	auto Arr2 = FABIDynamicArrayProperty<FABIStringProperty>{};
	DecodeProperties.Push(&Arr1);
	DecodeProperties.Push(&Arr2);
	ABI::Decode(Obj, DecodeProperties);

	for(auto Prop : *Arr1.GetValue())
	{
		for(auto InnerProp : *Prop.GetValue())
		{
			UE_LOG(LogTemp, Display, TEXT("The Value of the int is %i"), InnerProp.GetValue());
		}
	}

	for(auto Prop : *Arr2.GetValue())
	{
		UE_LOG(LogTemp, Display, TEXT("The Value of the string is %s"), *Prop.GetValue());
	}
	
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
