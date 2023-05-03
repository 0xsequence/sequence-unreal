#include "HexUtility.h"
#include "RLP.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestRLP, "Public.Tests.TestRLP",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestRLP::RunTest(const FString& Parameters)
{
	auto L1 = GetByteLength(0xF);
	auto L2 = GetByteLength(0xFFF);
	auto L3 = GetByteLength(0xFFFFF);
	auto L4 = GetByteLength(0xFFFFFFF);

	UE_LOG(LogTemp, Display, TEXT("%i %i %i %i"), L1, L2, L3, L4);

	auto Data = RLP::Encode(Itemize(new RLPItem[]
	{
		Itemize(new RLPItem[]{}, 0),
		Itemize(new RLPItem[]
		{
			Itemize(new RLPItem[]{}, 0),
		}, 1),
		Itemize(new RLPItem[]
		{
			Itemize(new RLPItem[]{}, 0),
			Itemize(new RLPItem[]
			{
				Itemize(new RLPItem[]{}, 0),
			}, 1),
		}, 2),
	}, 3));
	UE_LOG(LogTemp, Display, TEXT("%s"), *BinaryToHexString(Data));
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
