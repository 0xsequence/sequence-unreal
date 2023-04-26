#include "Crypto.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "secp256k1Library/secp256k1/secp256k1.h"
#include "secp256k1Library/secp256k1/secp256k1_extrakeys.h"

PublicKey GetPublicKey(PrivateKey PrivateKey)
{
	secp256k1_context* Context = secp256k1_context_create(SECP256K1_CONTEXT_NONE);
	secp256k1_keypair KeyPair;
	secp256k1_pubkey Pubkey;
	secp256k1_keypair_create(Context,&KeyPair, PrivateKey);
	secp256k1_keypair_pub(Context, &Pubkey, &KeyPair);
	secp256k1_context_destroy(Context);

	// Copy into our public key
	PublicKey PublicKey = new uint8(GPublicKeyByteLength);
	for(auto i = 0; i < GPublicKeyByteLength; i++)
	{
		PublicKey[i] = Pubkey.data[i];
	}

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
