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
	auto CHAIN_ID = 1337; //1337;
	auto DATA = HexStringToBinary("");
	
	auto provider = Provider("http://localhost:8545/");
	provider.SendRawTransaction("0x" +
		FEthTransaction{
			FBlockNonce::From("0x09"),
			HexStringToBinary("0x000005f5e100"),
			HexStringToBinary("0x5208"),
			 FAddress::From("0xb78777860637d56543da23312c7865024833f7d1"),
			HexStringToBinary("0x16345785d8a0000"),
			HexStringToBinary("0x10"),
		}.GetSignedTransaction(PRIVATE_KEY, CHAIN_ID)
		.ToHex()
	 ).Wait();

	provider.SendRawTransaction("0x" +
		FEthTransaction{
			FBlockNonce::From("0x00"),
			HexStringToBinary("0x000005f5e100"),
			HexStringToBinary("0x5208"),
			 FAddress::From("0xb78777860637d56543da23312c7865024833f7d1"),
			HexStringToBinary("0x16345785d8a0000"),
			HexStringToBinary("0x10"),
		}.GetSignedTransaction(PRIVATE_KEY, CHAIN_ID)
		.ToHex()
	 ).Wait();

	provider.SendRawTransaction("0x" +
		FEthTransaction{
			FBlockNonce::From("0x10"),
			HexStringToBinary("0x000005f5e100"),
			HexStringToBinary("0x5208"),
			 FAddress::From("0x0000000000000000000000000000000000000000"),
			HexStringToBinary("0x16345785d8a0000"),
			HexStringToBinary("0x10"),
		}.GetSignedTransaction(PRIVATE_KEY, CHAIN_ID)
		.ToHex()
	 ).Wait();

	provider.SendRawTransaction("0x" +
		FEthTransaction{
			FBlockNonce::From("0x10"),
			HexStringToBinary("0x000005f5e100"),
			HexStringToBinary("200000"),
			 FAddress::From("3535353535353535353535353535353535353535"),
			VALUE,
			DATA,
		}.GetSignedTransaction(PRIVATE_KEY, CHAIN_ID)
		.ToHex()
	 ).Wait();

	
	
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
