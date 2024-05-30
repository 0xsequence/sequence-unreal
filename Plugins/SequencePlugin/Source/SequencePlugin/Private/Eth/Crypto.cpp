// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "Eth/Crypto.h"
#include "Util/HexUtility.h"
#include "Eth/RLP.h"
#include "Types/BinaryData.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/CurvePoint.hpp"



FPublicKey GetPublicKey(FPrivateKey PrivateKey)
{
	Uint256 PrivateKey256 = Uint256(PrivateKey.Ptr());
	CurvePoint CurvePoint = CurvePoint::privateExponentToPublicPoint(PrivateKey256);
	FPublicKey PublicKey = FPublicKey::New();
	CurvePoint.x.getBigEndianBytes(PublicKey.Ptr());
	CurvePoint.y.getBigEndianBytes(&PublicKey.Ptr()[FPublicKey::Size / 2]);
	return PublicKey;
}

FAddress GetAddress(FPublicKey PublicKey)
{
	FHash256 Hash = FHash256::New();
	Keccak256::getHash(PublicKey.Ptr(), FPublicKey::Size, Hash.Ptr());
	FAddress Address = FAddress::New();
	for(uint32 i = 0; i < FAddress::Size; i++)
	{
		Address.Ptr()[i] = Hash.Ptr()[i + 12];
	}
	return Address;
}

FHash256 GetKeccakHash(FBinaryData& Data)
{
	FHash256 HashData = FHash256::New();
	Keccak256::getHash(Data.Ptr(), Data.GetLength(), HashData.Ptr());
	return HashData;
}

FAddress GetContractAddress(FAddress Sender, FBlockNonce Nonce)
{
	FString NonceStr = TrimHex(Nonce.ToHex());
	RLPItem* RLPArray = new RLPItem[]{Itemize(Sender), Itemize(HexStringToBinary(NonceStr))};
	RLPItem Items = Itemize(RLPArray, 2);
	FUnsizedData Data = RLP::Encode(Items);
	FHash256 Hash = GetKeccakHash(Data);

	const FAddress Address = FAddress::New();
	for(uint32 i = 0; i < FAddress::Size; i++)
	{
		Address.Ptr()[i] = Hash.Ptr()[i + 12];
	}

	//cleanup
	delete[] RLPArray;

	return Address;
}
