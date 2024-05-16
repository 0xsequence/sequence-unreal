// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "Eth/Crypto.h"
#include "Util/HexUtility.h"
#include "Provider.h"
#include "Eth/RLP.h"
#include "Misc/AutomationTest.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Ecdsa.hpp"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestTransaction, "Public.Tests.TestTransaction",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestTransaction::RunTest(const FString& Parameters)
{
	/*
	
	auto byte_code = "0x608060405234801561001057600080fd5b50610997806100206000396000f3fe608060405234801561001057600080fd5b50600436106100be5760003560e01c80633950935111610076578063a457c2d71161005b578063a457c2d7146102f0578063a9059cbb14610329578063dd62ed3e14610362576100be565b8063395093511461028457806370a08231146102bd576100be565b806323b872dd116100a757806323b872dd1461012a5780632e72102f1461016d578063378934b41461024b576100be565b8063095ea7b3146100c357806318160ddd14610110575b600080fd5b6100fc600480360360408110156100d957600080fd5b5073ffffffffffffffffffffffffffffffffffffffff813516906020013561039d565b604080519115158252519081900360200190f35b6101186103b3565b60408051918252519081900360200190f35b6100fc6004803603606081101561014057600080fd5b5073ffffffffffffffffffffffffffffffffffffffff8135811691602081013590911690604001356103b9565b6102496004803603606081101561018357600080fd5b81019060208101813564010000000081111561019e57600080fd5b8201836020820111156101b057600080fd5b803590602001918460208302840111640100000000831117156101d257600080fd5b9193909273ffffffffffffffffffffffffffffffffffffffff8335169260408101906020013564010000000081111561020a57600080fd5b82018360208201111561021c57600080fd5b8035906020019184602083028401116401000000008311171561023e57600080fd5b509092509050610417565b005b6102496004803603604081101561026157600080fd5b5073ffffffffffffffffffffffffffffffffffffffff8135169060200135610509565b6100fc6004803603604081101561029a57600080fd5b5073ffffffffffffffffffffffffffffffffffffffff8135169060200135610517565b610118600480360360208110156102d357600080fd5b503573ffffffffffffffffffffffffffffffffffffffff1661055a565b6100fc6004803603604081101561030657600080fd5b5073ffffffffffffffffffffffffffffffffffffffff8135169060200135610582565b6100fc6004803603604081101561033f57600080fd5b5073ffffffffffffffffffffffffffffffffffffffff81351690602001356105c5565b6101186004803603604081101561037857600080fd5b5073ffffffffffffffffffffffffffffffffffffffff813581169160200135166105d2565b60006103aa33848461060a565b50600192915050565b60025490565b60006103c68484846106b9565b73ffffffffffffffffffffffffffffffffffffffff841660009081526001602090815260408083203380855292529091205461040d91869161040890866107ac565b61060a565b5060019392505050565b60005b818110156105015785858281811061042e57fe5b9050602002013573ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff1663a9059cbb8585858581811061047357fe5b905060200201356040518363ffffffff1660e01b8152600401808373ffffffffffffffffffffffffffffffffffffffff16815260200182815260200192505050602060405180830381600087803b1580156104cd57600080fd5b505af11580156104e1573d6000803e3d6000fd5b505050506040513d60208110156104f757600080fd5b505060010161041a565b505050505050565b6105138282610823565b5050565b33600081815260016020908152604080832073ffffffffffffffffffffffffffffffffffffffff8716845290915281205490916103aa91859061040890866108e6565b73ffffffffffffffffffffffffffffffffffffffff1660009081526020819052604090205490565b33600081815260016020908152604080832073ffffffffffffffffffffffffffffffffffffffff8716845290915281205490916103aa91859061040890866107ac565b60006103aa3384846106b9565b73ffffffffffffffffffffffffffffffffffffffff918216600090815260016020908152604080832093909416825291909152205490565b73ffffffffffffffffffffffffffffffffffffffff821661062a57600080fd5b73ffffffffffffffffffffffffffffffffffffffff831661064a57600080fd5b73ffffffffffffffffffffffffffffffffffffffff808416600081815260016020908152604080832094871680845294825291829020859055815185815291517f8c5be1e5ebec7d5bd14f71427d1e84f3dd0314c0f7b2291e5b200ac8c7c3b9259281900390910190a3505050565b73ffffffffffffffffffffffffffffffffffffffff82166106d957600080fd5b73ffffffffffffffffffffffffffffffffffffffff831660009081526020819052604090205461070990826107ac565b73ffffffffffffffffffffffffffffffffffffffff808516600090815260208190526040808220939093559084168152205461074590826108e6565b73ffffffffffffffffffffffffffffffffffffffff8084166000818152602081815260409182902094909455805185815290519193928716927fddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef92918290030190a3505050565b60008282111561081d57604080517f08c379a000000000000000000000000000000000000000000000000000000000815260206004820152601760248201527f536166654d617468237375623a20554e444552464c4f57000000000000000000604482015290519081900360640190fd5b50900390565b73ffffffffffffffffffffffffffffffffffffffff821661084357600080fd5b60025461085090826108e6565b60025573ffffffffffffffffffffffffffffffffffffffff821660009081526020819052604090205461088390826108e6565b73ffffffffffffffffffffffffffffffffffffffff83166000818152602081815260408083209490945583518581529351929391927fddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef9281900390910190a35050565b60008282018381101561095a57604080517f08c379a000000000000000000000000000000000000000000000000000000000815260206004820152601660248201527f536166654d617468236164643a204f564552464c4f5700000000000000000000604482015290519081900360640190fd5b939250505056fea26469706673582212201e8282683b3b9580e5722aa29d3e976acdd4cd35c3e88cdd1abb688867d0547a64736f6c63430007040033";

	// constants for the transaction
	
	auto NONCE = "1";
	auto GASPRICE= "30000"; // "0x4A817C800";
	auto GASLIMIT = "2000000"; // Has to be high enough for it to serialize properly? // TODO LOOK AT THIS
	auto TO = "0000000000000000000000000000000000000000"; // "3535353535353535353535353535353535353535";
	auto VALUE = "1"; // 0xDE0B6B3A7640000;
	auto PRIVATE_KEY = FPrivateKey::From("abc0000000000000000000000000000000000000000000000000000000000001");
	auto CHAIN_ID = 1337;
	auto DATA = byte_code;

	NONCE = "0x00";
	GASPRICE = "0x005f5e10022";
	GASLIMIT = "0x5203822";
	//TO = "0xb78777860637d56543da23312c7865024833f7d1";
	VALUE = "0x16345785d8a0000";
	//DATA = "0x";
	//CHAIN_ID = 1;
	
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
		Itemize(HexStringToBinary(GASPRICE).Trim()), // GasPrice
		Itemize(HexStringToBinary(GASLIMIT)), // GasLimit
		Itemize(HexStringToBinary(TO)), // To
		Itemize(HexStringToBinary(VALUE)), // Value
		Itemize(HexStringToBinary(DATA)), // Data
		Itemize(HexStringToBinary(IntToHexString(CHAIN_ID))), // V
		Itemize(HexStringToBinary("")), // R 
		Itemize(HexStringToBinary("")), // S
	}, 9);

	auto EncodedSigningData = RLP::Encode(item); // RLP Encoder creates data to be hashed
	//EncodedSigningData = HexStringToBinary("eb808405f5e10082520894b78777860637d56543da23312c7865024833f7d188016345785d8a000080808080");
	
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
	uint16 recoveryParameter;
	
	auto msgHash = Sha256Hash(SigningHash.Arr, FHash256::Size); //"dc77b4d97ddddcb8ba2ab7ed304cf6449a6eebde8322ee2819cc90975c15c65a");//); // We must convert our hash to Nayuki's Sha256Hash 

	// Usually EDSCA uses a random nonce value that has to be generated
	// Nowadays crypto libraries get this random nonce deterministically from the private key and hash
	// That is what signWithHmacNonce does

	auto signed_success = false;
	signed_success = Ecdsa::signWithHmacNonce(Uint256(PRIVATE_KEY.Arr), msgHash, R, S, recoveryParameter);
	
	R.getBigEndianBytes(MyR); // transfer data
	S.getBigEndianBytes(MyS); // transfer data
	
	
	// See https://eips.ethereum.org/EIPS/eip-155#parameters
	uint16 V = CHAIN_ID * 2 + 35 + recoveryParameter; 

	// Now we just need to combine our data into one RLP encoded hex string and send it
	
	//signing data
	auto FullTransaction = Itemize(new RLPItem[]
	{
		Itemize(HexStringToBinary(NONCE)), // Nonce
		Itemize(HexStringToBinary(GASPRICE).Trim()), // GasPrice
		Itemize(HexStringToBinary(GASLIMIT)), // GasLimit
		Itemize(HexStringToBinary(TO)), // To
		Itemize(HexStringToBinary(VALUE)), // Value
		Itemize(HexStringToBinary(DATA)), // Data
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
	*/
	
	return true; 
}
