#include "HexUtility.h"
#include "RLP.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestRLP, "Public.Tests.TestRLP",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestRLP::RunTest(const FString& Parameters)
{
	/*auto L1 = GetByteLength(0xF);
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
	*/
	
	auto encoded = RLP::Encode(Itemize(HexStringToBinary("80d868b36ea76878df071e4a4f868942cf79224532e5dcf88eb8fc56b2aa055e"))).ToHex();
	UE_LOG(LogTemp, Display, TEXT("%s"), *encoded);

	auto item2 = Itemize(new RLPItem[]
	{
		Itemize(HexStringToBinary("0x09")), // Nonce
		Itemize(HexStringToBinary("0x4A817C800")), // GasPrice
		Itemize(HexStringToBinary("0x5208")), // GasLimit
		Itemize(HexStringToBinary("0x3535353535353535353535353535353535353535")), // To
		Itemize(HexStringToBinary("0xDE0B6B3A7640000")), // Value
		Itemize(HexStringToBinary("")), // Data
		Itemize(HexStringToBinary("25")), // V
		Itemize(HexStringToBinary("28ef61340bd939bc2195fe537567866003e1a15d3c71ff63e1590620aa636276")), // R 
		Itemize(HexStringToBinary("67cbe9d8997f761aecb703304b3800ccf555c9f3dc64214b297fb1966a3b6d83")),  // S
	}, 9);
	
	encoded = RLP::Encode(item2).ToHex();
	UE_LOG(LogTemp, Display, TEXT("%s"), *encoded);
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
