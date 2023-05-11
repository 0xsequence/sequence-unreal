#include "ABI.h"
#include "Crypto.h"
#include "HexUtility.h"
#include "Provider.h"
#include "RLP.h"
#include "Misc/AutomationTest.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Ecdsa.hpp"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestTransaction, "Public.Tests.TestTransaction",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestTransaction::RunTest(const FString& Parameters)
{

	// constants for the transaction
	auto NONCE = "4";
	auto GASPRICE= "1"; // "0x4A817C800";
	auto GASLIMIT = "2000000";
	auto TO = "3535353535353535353535353535353535353535";
	auto VALUE = "1"; // 0xDE0B6B3A7640000;
	auto PRIVATE_KEY = FPrivateKey::From("abc0000000000000000000000000000000000000000000000000000000000001");
	auto CHAIN_ID = 1337;
	
	// This is the first private key in the hardhat config
	// should correspond to address 0xc683a014955b75F5ECF991d4502427c8fa1Aa249
	// We can see in the logging that we indeed generate the same address

	// currently following the standard set by https://eips.ethereum.org/EIPS/eip-155
	auto USING_EIP155 = true;
	
	// Make the test pass by returning tru e, or fail by returning false.

	// signing data
	
	RLPItem item;

	item = Itemize(new RLPItem[]
	{
		Itemize(HexStringToBinary(NONCE)), // Nonce
		Itemize(HexStringToBinary(GASPRICE)), // GasPrice
		Itemize(HexStringToBinary(GASLIMIT)), // GasLimit
		Itemize(HexStringToBinary(TO)), // To
		Itemize(HexStringToBinary(VALUE)), // Value
		Itemize(HexStringToBinary("")), // Data
		Itemize(HexStringToBinary(IntToHexString(CHAIN_ID))), // V
		Itemize(HexStringToBinary("")), // R 
		Itemize(HexStringToBinary("")), // S
	}, 9);

	auto EncodedSigningData = RLP::Encode(item); // RLP Encoder creates data to be hashed
	
	// signing hash
	// Verify using https://emn178.github.io/online-tools/keccak_256.html
	// Looks correct to me - Jan
	FHash256 SigningHash = FHash256::New();
	Keccak256::getHash(EncodedSigningData.Arr, EncodedSigningData.GetLength(), SigningHash.Arr);

	//signing transaction
	//PrivateKey Privkey = HexStringToHash(GPrivateKeyByteLength, PRIVATE_KEY);
	FPublicKey PubKey = GetPublicKey(PRIVATE_KEY);
	FAddress Addr = GetAddress(PubKey);
	uint8 MyR[FHash256::Size]; // Buffer for R to be transferred to
	uint8 MyS[FHash256::Size]; // Buffer for S to be transferred to
	Uint256 R, S; // These have to be Nayuki's special Uint256s
	
	auto msgHash = Sha256Hash(SigningHash.Arr, FHash256::Size); //"dc77b4d97ddddcb8ba2ab7ed304cf6449a6eebde8322ee2819cc90975c15c65a");//); // We must convert our hash to Nayuki's Sha256Hash 

	// Usually EDSCA uses a random nonce value that has to be generated
	// Nowadays crypto libraries get this random nonce deterministically from the private key and hash
	// That is what signWithHmacNonce does

	auto signed_success = false;
	signed_success = Ecdsa::signWithHmacNonce(Uint256(PRIVATE_KEY.Arr), msgHash, R, S);
	
	R.getBigEndianBytes(MyR); // transfer data
	S.getBigEndianBytes(MyS); // transfer data
	
	
	// See https://eips.ethereum.org/EIPS/eip-155#parameters
	uint16 V = (MyR[FHash256::Size - 1] % 2) + CHAIN_ID * 2 + 35; 

	// Now we just need to combine our data into one RLP encoded hex string and send it
	
	//signing data
	auto FullTransaction = Itemize(new RLPItem[]
	{
		Itemize(HexStringToBinary(NONCE)), // Nonce
		Itemize(HexStringToBinary(GASPRICE)), // GasPrice
		Itemize(HexStringToBinary(GASLIMIT)), // GasLimit
		Itemize(HexStringToBinary(TO)), // To
		Itemize(HexStringToBinary(VALUE)), // Value
		Itemize(HexStringToBinary("")), // Data
		Itemize(HexStringToBinary(IntToHexString(V))), // V
		Itemize(HexStringToBinary(FHash256::From(MyR).ToHex())), // R 
		Itemize(HexStringToBinary(FHash256::From(MyS).ToHex())),  // S
	}, 9);

	auto SignedTransaction = RLP::Encode(FullTransaction);
	auto SignedTransactionString = SignedTransaction.ToHex();

	auto provider = Provider("http://localhost:8545/");
	provider.SendRawTransaction("0x" + SignedTransactionString);


	// This will tell us if the crypto library is internally consistent, that is, it can verify that our transaction
	//  is indeed signed using the same private key
	// It has been looking correct so far - Jan
	auto CurvePointPubKey = CurvePoint::privateExponentToPublicPoint(Uint256(PRIVATE_KEY.Arr)); // Convert private key to x, y point on secp256k1 curve
	auto isVerified = Ecdsa::ecdsa_verify(CurvePointPubKey, msgHash, Uint256(MyR), Uint256(MyS));
	if(isVerified)
	{
		UE_LOG(LogTemp, Display, TEXT("Verified!"));
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Not Verified!"));
	}

	// Print Info for afterwards
	// Verify RLP encoded data using https://flightwallet.github.io/decode-eth-tx/
	// Everything seems to match up correctly except that the R and S values generate the wrong address :( - Jan
	{
		UE_LOG(LogTemp, Display, TEXT("ENCODED SIGNING DATA: %s"), *EncodedSigningData.ToHex());
		UE_LOG(LogTemp, Display, TEXT("ENCODED SIGNING HASH: %s"), *SigningHash.ToHex());
		UE_LOG(LogTemp, Display, TEXT("SIGNED TRANSACTION STRING: %s"), *SignedTransactionString);
		UE_LOG(LogTemp, Display, TEXT("PRIVKEY: %s"), *PRIVATE_KEY.ToHex());
		UE_LOG(LogTemp, Display, TEXT("PUBKEY: %s"), *PubKey.ToHex());
		UE_LOG(LogTemp, Display, TEXT("ADDR: %s"), *Addr.ToHex());
		UE_LOG(LogTemp, Display, TEXT("V: %s"), *IntToHexString(V));
		UE_LOG(LogTemp, Display, TEXT("R: %s"), *FHash256::From(MyR).ToHex());
		UE_LOG(LogTemp, Display, TEXT("S: %s"), *FHash256::From(MyS).ToHex());
	}

	return true;
}
