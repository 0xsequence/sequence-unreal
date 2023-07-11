#include "ABI/ABIStaticFixedArray.h"
#include "ABI/ABITypes.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestNestedFixedArray, "Public.Tests.ABI.TestNestedFixedArray",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestNestedFixedArray::RunTest(const FString& Parameters)
{
	
	FABIUInt32Property Number = FABIUInt32Property(42);
	TArray<FABIUInt32Property> Numbers = TArray{Number};
	FABIStaticFixedArrayProperty<FABIUInt32Property, 1> Array = FABIStaticFixedArrayProperty<FABIUInt32Property, 1>(&Numbers);
	TArray<FABIStaticFixedArrayProperty<FABIUInt32Property, 1>> Arrays = TArray{Array};
	FABIStaticFixedArrayProperty<FABIStaticFixedArrayProperty<FABIUInt32Property, 1>, 1> OuterArray = FABIStaticFixedArrayProperty<FABIStaticFixedArrayProperty<FABIUInt32Property, 1>, 1>{&Arrays};
	TArray<FABIProperty*> Props = TArray<FABIProperty*>{&OuterArray};
	auto Data = ABI::Encode("", Props);
	UE_LOG(LogTemp, Display, TEXT("Encoded value %s"), *Data.ToHex());

	Number.SetValue(0);
	ABI::Decode(Data, Props);
	auto NumberVal = (*(*OuterArray.GetValue())[0].GetValue())[0].GetValue();
	UE_LOG(LogTemp, Display, TEXT("Decoded value is %i"), NumberVal);
	if(NumberVal != 42) return false;
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
