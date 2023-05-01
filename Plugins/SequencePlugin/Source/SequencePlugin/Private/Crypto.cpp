#include "Crypto.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/CurvePoint.hpp"

PublicKey GetPublicKey(PrivateKey PrivateKey)
{
	auto PrivKey = Uint256(PrivateKey);
	auto PubKey = CurvePoint::privateExponentToPublicPoint(PrivKey);
	PubKey.normalize();

	PublicKey PublicKey = new uint8[GPublicKeyByteLength];

	
	PubKey.x.getBigEndianBytes(PublicKey);
	PubKey.y.getBigEndianBytes(&PublicKey[GPublicKeyByteLength / 2]);

	return PublicKey;
}

Address GetAddress(PrivateKey PublicKey)
{
	Hash256 Hash = new uint8[GHash256ByteLength];
	Keccak256::getHash(PublicKey, GPublicKeyByteLength, Hash);

	Address Address = new uint8[GAddressByteLength];
	for(auto i = 0; i < 40; i++)
	{
		Address[i] = Hash[i + 12];
	}

	delete [] Hash;
	return Address;
}
