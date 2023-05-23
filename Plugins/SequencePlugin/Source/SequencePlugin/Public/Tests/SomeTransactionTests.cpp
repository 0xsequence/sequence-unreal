#include "BinaryData.h"
#include "HexUtility.h"
#include "Provider.h"
#include "FEthTransaction.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(SomeTransactionTests, "Public.Tests.SomeTransactionTests",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool SomeTransactionTests::RunTest(const FString& Parameters)
{
	// constants for the transaction
	auto NONCE = FBlockNonce::From("5");
	auto GASPRICE= HexStringToBinary("12"); // "0x4A817C800";
	auto GASLIMIT = HexStringToBinary("200000");
	auto TO = FAddress::From("3535353535353535353535353535353535353535");
	auto VALUE = HexStringToBinary("1"); // 0xDE0B6B3A7640000;
	auto PRIVATE_KEY = FPrivateKey::From("abc0000000000000000000000000000000000000000000000000000000000001");
	auto CHAIN_ID = 1337;
	auto DATA = HexStringToBinary("");
	
	auto provider = Provider("http://localhost:8545/");
	provider.SendRawTransaction("0x" +
		FEthTransaction{
			NONCE,
			GASPRICE,
			GASLIMIT,
			TO,
			VALUE,
			DATA,
		}.GetSignedTransaction(PRIVATE_KEY, CHAIN_ID)
		.ToHex()
	 );
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
