#include "HexUtility.h"
#include "Provider.h"
#include "Transaction.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(SomeTransactionTests, "Public.Tests.SomeTransactionTests",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool SomeTransactionTests::RunTest(const FString& Parameters)
{
	auto NONCE = HexStringToHash(GBlockNonceByteLength, "9");
	auto GASPRICE= HexStringtoBinary("0x1"); // "0x4A817C800";
	auto GASLIMIT = HexStringtoBinary("0x1");
	auto TO = HexStringToHash(GAddressByteLength, "0x6F5Ddb00e3cb99Dfd9A07885Ea91303629D1DA94"); //"0x3535353535353535353535353535353535353535");
	auto VALUE = HexStringtoBinary("0x1"); // 0xDE0B6B3A7640000;
	auto PRIVATE_KEY = HexStringToHash(GPrivateKeyByteLength, "0xabc0000000000000000000000000000000000000000000000000000000000001");
	auto DATA = HexStringtoBinary("0x0");
	auto CHAIN_ID = 1337;
	
	auto provider = Provider("http://localhost:8545/");
	provider.SendRawTransaction("0x" + BinaryToHexString(Transaction{
		NONCE,
		GASPRICE,
		GASLIMIT,
		TO,
		VALUE,
		DATA,
	}.GetSignedTransaction(PRIVATE_KEY, CHAIN_ID)));
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
