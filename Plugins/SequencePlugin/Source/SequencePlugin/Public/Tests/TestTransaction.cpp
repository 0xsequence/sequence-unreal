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
	auto NONCE = "0x09";
	auto GASPRICE= "0x1"; // "0x4A817C800";
	auto GASLIMIT = "0x2";
	auto TO = "0x3535353535353535353535353535353535353535";
	auto VALUE = "0x1"; // 0xDE0B6B3A7640000;
	auto PRIVATE_KEY = "0xabc0000000000000000000000000000000000000000000000000000000000001";

	// This is the first private key in the hardhat config
	// should correspond to address 0xc683a014955b75F5ECF991d4502427c8fa1Aa249
	// We can see in the logging that we indeed generate the same address

	// currently following the standard set by https://eips.ethereum.org/EIPS/eip-155
	auto USING_EIP155 = true;
	
	// Make the test pass by returning tru e, or fail by returning false.

	// signing data
	
	RLPItem item;

	if(USING_EIP155)
	{
		item = Itemize(new RLPItem[]
	    {
			Itemize(HexStringtoBinary(NONCE)), // Nonce
			Itemize(HexStringtoBinary(GASPRICE)), // GasPrice
			Itemize(HexStringtoBinary(GASLIMIT)), // GasLimit
			Itemize(HexStringtoBinary(TO)), // To
			Itemize(HexStringtoBinary(VALUE)), // Value
			Itemize(HexStringtoBinary("0x")), // Data
			Itemize(HexStringtoBinary("0x1")), // V
			Itemize(HexStringtoBinary("0x")), // R 
			Itemize(HexStringtoBinary("0x")), // S
	    }, 9);
	}
	else
	{
		item = Itemize(new RLPItem[]
		{
			Itemize(HexStringtoBinary(NONCE)), // Nonce
			Itemize(HexStringtoBinary(GASPRICE)), // GasPrice
			Itemize(HexStringtoBinary(GASLIMIT)), // GasLimit
			Itemize(HexStringtoBinary(TO)), // To
			Itemize(HexStringtoBinary(VALUE)), // Value
			Itemize(HexStringtoBinary("0x")), // Data
		}, 6);
	}

	auto EncodedSigningData = RLP::Encode(item); // RLP Encoder creates data to be hashed

	// Trying something from https://eips.ethereum.org/EIPS/eip-191
	// The first character is a placeholder that has to be replaced with x19
	auto Msg = "XEthereum Signed Message:\n" + FString::FromInt(EncodedSigningData.ByteLength) + BinaryToHexString(EncodedSigningData);
	auto EncodedMsg = String_to_UTF8(Msg);
	EncodedMsg.Data[0] = 0x19;
	
	// signing hash
	// Verify using https://emn178.github.io/online-tools/keccak_256.html
	// Looks correct to me - Jan
	Hash256 SigningHash = new uint8[GHash256ByteLength];
	Keccak256::getHash(EncodedMsg.Data, EncodedMsg.ByteLength, SigningHash);

	//signing transaction
	PrivateKey Privkey = HexStringToHash(GPrivateKeyByteLength, PRIVATE_KEY);
	PublicKey PubKey = GetPublicKey(Privkey);
	Address Addr = GetAddress(PubKey);
	uint8 MyR[GHash256ByteLength]; // Buffer for R to be transferred to
	uint8 MyS[GHash256ByteLength]; // Buffer for S to be transferred to
	Uint256 R, S; // These have to be Nayuki's special Uint256s
	auto msgHash = Sha256Hash(SigningHash, GHash256ByteLength); // We must convert our hash to Nayuki's Sha256Hash 

	// Usually EDSCA uses a random nonce value that has to be generated
	// Nowadays crypto libraries get this random nonce deterministically from the private key and hash
	// That is what signWithHmacNonce does
	auto b = Ecdsa::signWithHmacNonce(Uint256(PRIVATE_KEY), msgHash, R, S); 
	R.getBigEndianBytes(MyR); // transfer data
	S.getBigEndianBytes(MyS); // transfer data
	
	auto ChainId = 1337;
	uint16 V;

	// See https://eips.ethereum.org/EIPS/eip-155#parameters
	if(USING_EIP155)
	{
		V = (MyR[GHash256ByteLength - 1] % 2) + ChainId * 2 + 35; 
	}
	else
	{
		V = (MyR[GHash256ByteLength - 1] % 2) + 27;
	}

	// Now we just need to combine our data into one RLP encoded hex string and send it
	
	//signing data
	auto FullTransaction = Itemize(new RLPItem[]
	{
		Itemize(HexStringtoBinary(NONCE)), // Nonce
		Itemize(HexStringtoBinary(GASPRICE)), // GasPrice
		Itemize(HexStringtoBinary(GASLIMIT)), // GasLimit
		Itemize(HexStringtoBinary(TO)), // To
		Itemize(HexStringtoBinary(VALUE)), // Value
		Itemize(HexStringtoBinary("")), // Data
		Itemize(HexStringtoBinary(IntToHexString(V))), // V
		Itemize(HexStringtoBinary(Hash256ToHexString(MyR))), // R 
		Itemize(HexStringtoBinary(Hash256ToHexString(MyS))),  // S
	}, 9);

	auto SignedTransaction = RLP::Encode(FullTransaction);
	auto SignedTransactionString = BinaryToHexString(SignedTransaction);

	auto provider = Provider("http://localhost:8545/");
	provider.SendRawTransaction("0x" + SignedTransactionString);


	// This will tell us if the crypto library is internally consistent, that is, it can verify that our transaction
	//  is indeed signed using the same private key
	// It has been looking correct so far - Jan
	auto CurvePointPubKey = CurvePoint::privateExponentToPublicPoint(Uint256(PRIVATE_KEY)); // Convert private key to x, y point on secp256k1 curve
	auto isVerified = Ecdsa::ecdsa_verify(CurvePointPubKey, msgHash, R, S);
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
		UE_LOG(LogTemp, Display, TEXT("ENCODED SIGNING DATA: %s"), *BinaryToHexString(EncodedSigningData));
		UE_LOG(LogTemp, Display, TEXT("ENCODED SIGNING HASH: %s"), *Hash256ToHexString(SigningHash));
		UE_LOG(LogTemp, Display, TEXT("SIGNED TRANSACTION STRING: %s"), *SignedTransactionString);
		UE_LOG(LogTemp, Display, TEXT("PUBKEY: %s"), *HashToHexString(GPublicKeyByteLength, PubKey));
		UE_LOG(LogTemp, Display, TEXT("ADDR: %s"), *HashToHexString(GAddressByteLength, Addr));
		UE_LOG(LogTemp, Display, TEXT("V: %s"), *IntToHexString(V));
		UE_LOG(LogTemp, Display, TEXT("R: %s"), *Hash256ToHexString(MyR));
		UE_LOG(LogTemp, Display, TEXT("S: %s"), *Hash256ToHexString(MyS));
	}

	return true;
}
