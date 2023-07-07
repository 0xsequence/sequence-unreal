#include "Crypto.h"
#include "Types/BinaryData.h"
#include "FEthTransaction.h"
#include "Provider.h"
#include "Misc/AutomationTest.h"
#include "Types/ContractCall.h"
#include "ABI/ABI.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "ABI/ABITypes.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(LargeTest, "Public.Tests.LargeTest",
                                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool LargeTest::RunTest(const FString& Parameters)
{
	auto provider = Provider("http://localhost:3000/");
	//deploy smart contract
	//make sure nonce is 0 first time
	//verify hash-receipt
	//make view smart contract call (make sure owner balance is 100)
	//make view smart contract call (make sure balance for your account is 0)
	//make refill call
	//make purchase call
	//deploy from different private key
	//deploy different code

	return true;
}
