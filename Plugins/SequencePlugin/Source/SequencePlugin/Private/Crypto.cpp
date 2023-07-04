#include "Crypto.h"

#include "RLP.h"
#include "Types/BinaryData.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/CurvePoint.hpp"

FPublicKey GetPublicKey(FPrivateKey PrivateKey)
{
	auto PrivKey = Uint256(PrivateKey.Arr);
	auto PubKey = CurvePoint::privateExponentToPublicPoint(PrivKey);
	FPublicKey PublicKey = FPublicKey::New();
	PubKey.x.getBigEndianBytes(PublicKey.Arr);
	PubKey.y.getBigEndianBytes(&PublicKey.Arr[FPublicKey::GetSize() / 2]);
	return PublicKey;
}

FAddress GetAddress(FPublicKey PublicKey)
{
	auto Hash = FHash256::New();
	Keccak256::getHash(PublicKey.Arr, FPublicKey::GetSize(), Hash.Arr);
	FAddress Address = FAddress::New();
	for(auto i = 0; i < FAddress::GetSize(); i++)
	{
		Address.Arr[i] = Hash.Arr[i + 12];
	}

	Hash.Destroy();
	return Address;
}

FHash256 GetKeccakHash(FBinaryData& Data)
{
	auto HashData = FHash256::New();
	Keccak256::getHash(Data.Arr, Data.GetLength(), HashData.Arr);
	return HashData;
}

FAddress GetContractAddress(FAddress Sender, FBlockNonce Nonce)
{
	const auto RLPArray = new RLPItem[]{Itemize(Sender), Itemize(Nonce)};
	const auto Items = Itemize(RLPArray, 2);
	auto Data = RLP::Encode(Items);
	delete [] RLPArray;
	const auto Hash = GetKeccakHash(Data);

	const FAddress Address = FAddress::New();
	for(auto i = 0; i < FAddress::GetSize(); i++)
	{
		Address.Arr[i] = Hash.Arr[i + 12];
	}
	delete [] Hash.Arr;

	return Address;
}
