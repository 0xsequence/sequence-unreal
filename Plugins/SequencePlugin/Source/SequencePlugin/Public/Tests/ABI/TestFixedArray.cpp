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

	auto ArrayProp1 = FABIFixedArrayProperty<FABIUInt32Property, 3>(new FABIUInt32Property[]{NumberProp1, NumberProp2, NumberProp3});

	TArray<FABIProperty*> Properties;
    Properties.Push(&ArrayProp1);
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

	ABI::Decode(Obj, Properties);

	UE_LOG(LogTemp, Display, TEXT("The value is %i"), NumberProp1.GetValue());
	UE_LOG(LogTemp, Display, TEXT("The value is %i"), NumberProp1.GetValue());
	UE_LOG(LogTemp, Display, TEXT("The value is %i"), NumberProp1.GetValue());
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
