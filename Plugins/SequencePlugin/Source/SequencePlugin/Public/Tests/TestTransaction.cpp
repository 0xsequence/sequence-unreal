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

	
	// Make the test pass by returning tru e, or fail by returning false.

	// signing data
	// currently following 
	auto item = Itemize(new RLPItem[]
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

	auto EncodedSigningData = RLP::Encode(item);
	
	//signing hash
	Hash256 SigningHash = new uint8[GHash256ByteLength];
	Keccak256::getHash(EncodedSigningData.Data, EncodedSigningData.ByteLength, SigningHash);

	//signing transaction
	PrivateKey Privkey = HexStringToHash(GPrivateKeyByteLength, PRIVATE_KEY);
	PublicKey PubKey = GetPublicKey(Privkey);
	Address Addr = GetAddress(PubKey);
	uint8 MyR[GHash256ByteLength];
	uint8 MyS[GHash256ByteLength];
	Uint256 R, S;
	auto msgHash = Sha256Hash(SigningHash, GHash256ByteLength);
	
	auto b = Ecdsa::signWithHmacNonce(Uint256(PRIVATE_KEY), msgHash, R, S);
	R.getBigEndianBytes(MyR);
	S.getBigEndianBytes(MyS);
	auto ChainId = 1337;
	uint16 V = ((MyR[GHash256ByteLength - 1] + 0) % 2) + ChainId * 2 + 35; // See https://eips.ethereum.org/EIPS/eip-155#parameters
	
	//signing data
	auto item2 = Itemize(new RLPItem[]
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

	auto SignedTransaction = RLP::Encode(item2);
	auto SignedTransactionString = BinaryToHexString(SignedTransaction);

	auto provider = Provider("http://localhost:8545/");
	provider.SendRawTransaction("0x" + SignedTransactionString);

	auto CurvePointPubKey = CurvePoint::privateExponentToPublicPoint(Uint256(PRIVATE_KEY));
	auto isVerified = Ecdsa::ecdsa_verify(CurvePointPubKey, msgHash, R, S);

	if(isVerified)
	{
		UE_LOG(LogTemp, Display, TEXT("Verified!"));
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Not Verified!"));
	}

	// Print Info
	{
		UE_LOG(LogTemp, Display, TEXT("ENCODED SIGNING DATA: %s"), *BinaryToHexString(EncodedSigningData));
		UE_LOG(LogTemp, Display, TEXT("ENCODED SIGNING HASH: %s"), *Hash256ToHexString(SigningHash));
		UE_LOG(LogTemp, Display, TEXT("PUBKEY: %s"), *HashToHexString(GPublicKeyByteLength, PubKey));
		UE_LOG(LogTemp, Display, TEXT("ADDR: %s"), *HashToHexString(GAddressByteLength, Addr));
		UE_LOG(LogTemp, Display, TEXT("V: %s"), *IntToHexString(V));
		UE_LOG(LogTemp, Display, TEXT("R: %s"), *Hash256ToHexString(MyR));
		UE_LOG(LogTemp, Display, TEXT("S: %s"), *Hash256ToHexString(MyS));
	}

	return true;
}
