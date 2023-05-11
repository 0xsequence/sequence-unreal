#include "Crypto.h"

#include "BinaryData.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/CurvePoint.hpp"

FPublicKey GetPublicKey(FPrivateKey PrivateKey)
{
	auto PrivKey = Uint256(PrivateKey.Arr);
	auto PubKey = CurvePoint::privateExponentToPublicPoint(PrivKey);
	FPublicKey PublicKey = FPublicKey::New();
	PubKey.x.getBigEndianBytes(PublicKey.Arr);
	PubKey.y.getBigEndianBytes(&PublicKey.Arr[FPublicKey::Size / 2]);
	return PublicKey;
}

FAddress GetAddress(FPublicKey PublicKey)
{
	auto Hash = FHash256::New();
	Keccak256::getHash(PublicKey.Arr, FPublicKey::Size, Hash.Arr);
	FAddress Address = FAddress::New();
	for(auto i = 0; i < FAddress::Size; i++)
	{
		Address.Arr[i] = Hash.Arr[i + 12];
	}

	Hash.Destroy();
	return Address;
}
