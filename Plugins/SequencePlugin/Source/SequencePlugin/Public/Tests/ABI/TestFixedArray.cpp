#pragma warning(disable: 4018)
#include "Misc/AutomationTest.h"
#include "Util/HexUtility.h"
#include "ABI/ABI.h"
#include "ABI/ABITypes.h"
#include "ABI/ABIDynamicArray.h"
#include "ABI/ABIDynamicFixedArray.h"
#include "ABI/ABIStaticFixedArray.h"

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

	TArray<FABIUInt32Property> MyArray = TArray{NumberProp1, NumberProp2, NumberProp3};

	auto ArrayProp1 = FABIDynamicFixedArrayProperty<FABIUInt32Property, 3>(&MyArray);
	auto ArrayProp2 = FABIStaticFixedArrayProperty<FABIUInt32Property, 3>(&MyArray);

	{
		TArray<FABIProperty*> Properties;
		Properties.Push(&ArrayProp1);
		auto Obj = ABI::Encode("newFun(string,address)", Properties);
		uint32 BlockNum = (Obj.Length - GMethodIdByteLength) / GBlockByteLength;
	
		UE_LOG(LogTemp, Display, TEXT("The length of is %i"), BlockNum);

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

		ABI::Decode(Obj, Properties);

		UE_LOG(LogTemp, Display, TEXT("The value is %i"), NumberProp1.GetValue());
		if(NumberProp1.GetValue() != 1) return false;
		UE_LOG(LogTemp, Display, TEXT("The value is %i"), NumberProp2.GetValue());
		if(NumberProp2.GetValue() != 2) return false;
		UE_LOG(LogTemp, Display, TEXT("The value is %i"), NumberProp3.GetValue());
		if(NumberProp3.GetValue() != 3) return false;
	}

	{
		TArray<FABIProperty*> Properties;
		Properties.Push(&ArrayProp2);
		auto Obj = ABI::Encode("newFun(string,address)", Properties);
		uint32 BlockNum = (Obj.Length - GMethodIdByteLength) / GBlockByteLength;
	
		UE_LOG(LogTemp, Display, TEXT("The length of is %i"), BlockNum);

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

		ABI::Decode(Obj, Properties);

		UE_LOG(LogTemp, Display, TEXT("The value is %i"), NumberProp1.GetValue());
		if(NumberProp1.GetValue() != 1) return false;
		UE_LOG(LogTemp, Display, TEXT("The value is %i"), NumberProp2.GetValue());
		if(NumberProp2.GetValue() != 2) return false;
		UE_LOG(LogTemp, Display, TEXT("The value is %i"), NumberProp3.GetValue());
		if(NumberProp3.GetValue() != 3) return false;
	}
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
