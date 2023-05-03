#include "Crypto.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/CurvePoint.hpp"

PublicKey GetPublicKey(PrivateKey PrivateKey)
{
	auto PrivKey = Uint256(PrivateKey);
	auto PubKey = CurvePoint::privateExponentToPublicPoint(PrivKey);
	//PubKey.normalize(); not sure why this is here but just in case I'll leave it commented out
	PublicKey PublicKey = new uint8[GPublicKeyByteLength];	
	PubKey.x.getBigEndianBytes(PublicKey);
	PubKey.y.getBigEndianBytes(&PublicKey[GPublicKeyByteLength / 2]);


	return PublicKey;
}

Address GetAddress(PrivateKey PublicKey)
{
	Hash256 Hash = new uint8[GHash256ByteLength];
	Keccak256::getHash(PublicKey, GPublicKeyByteLength, Hash);
	Address addr = new uint8[GAddressByteLength];
	for(auto i = 0; i < GAddressByteLength; i++)
	{
		addr[i] = Hash[i + 12];
	}

	delete [] Hash;
	return addr;
}
