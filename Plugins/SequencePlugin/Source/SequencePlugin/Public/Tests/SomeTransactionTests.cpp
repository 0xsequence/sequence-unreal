#include "EthTransaction.h"
#include "Types/BinaryData.h"
#include "HexUtility.h"
#include "Provider.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(SomeTransactionTests, "Public.Tests.SomeTransactionTests",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool SomeTransactionTests::RunTest(const FString& Parameters)
{
	// constants for the transaction
	auto NONCE = FBlockNonce::From("9");
	auto GASPRICE= HexStringToBinary("4A817C800"); // "0x4A817C800";
	auto GASLIMIT = HexStringToBinary("5208");
	auto TO = FAddress::From("3535353535353535353535353535353535353535");
	auto VALUE = HexStringToBinary("de0b6b3a7640000"); // 0xDE0B6B3A7640000;
	auto PRIVATE_KEY = FPrivateKey::From("0x4646464646464646464646464646464646464646464646464646464646464646"); //"abc0000000000000000000000000000000000000000000000000000000000001");
	auto CHAIN_ID = 1; //1337;
	auto DATA = HexStringToBinary("");
	
	auto provider = Provider("http://localhost:8545/");
	provider.SendRawTransaction("0x" +
		FEthTransaction{
			FBlockNonce::From("0x09"),
			GASPRICE,
			GASLIMIT,
			 FAddress::From("3535353535353535353535353535353535353535"),
			VALUE,
			DATA,
		}.GetSignedTransaction(PRIVATE_KEY, CHAIN_ID)
		.ToHex()
	 );

	
	
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
