#include "Eth/Crypto.h"
#include "Util/HexUtility.h"
#include "Eth/RLP.h"
#include "Types/BinaryData.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/CurvePoint.hpp"

FPublicKey GetPublicKey(FPrivateKey PrivateKey)
{
	Uint256 PrivateKey256 = Uint256(PrivateKey.Arr);
	CurvePoint CurvePoint = CurvePoint::privateExponentToPublicPoint(PrivateKey256);
	FPublicKey PublicKey = FPublicKey::New();
	CurvePoint.x.getBigEndianBytes(PublicKey.Arr);
	CurvePoint.y.getBigEndianBytes(&PublicKey.Arr[FPublicKey::Size / 2]);
	return PublicKey;
}

FAddress GetAddress(FPublicKey PublicKey)
{
	FHash256 Hash = FHash256::New();
	Keccak256::getHash(PublicKey.Arr, FPublicKey::Size, Hash.Arr);
	FAddress Address = FAddress::New();
	for(uint32 i = 0; i < FAddress::Size; i++)
	{
		Address.Arr[i] = Hash.Arr[i + 12];
	}

	Hash.Destroy();
	return Address;
}

FHash256 GetKeccakHash(FBinaryData& Data)
{
	FHash256 HashData = FHash256::New();
	Keccak256::getHash(Data.Arr, Data.GetLength(), HashData.Arr);
	return HashData;
}

FAddress GetContractAddress(FAddress Sender, FBlockNonce Nonce)
{
	FString NonceStr = TrimHex(Nonce.ToHex());
	RLPItem* RLPArray = new RLPItem[]{Itemize(Sender), Itemize(HexStringToBinary(NonceStr))};
	RLPItem Items = Itemize(RLPArray, 2);
	FUnsizedData Data = RLP::Encode(Items);
	UE_LOG(LogTemp, Display, TEXT("Nonce %s RLP is %s"),*Nonce.ToHex(), *Data.ToHex())
	delete [] RLPArray;
	FHash256 Hash = GetKeccakHash(Data);

	const FAddress Address = FAddress::New();
	for(uint32 i = 0; i < FAddress::Size; i++)
	{
		Address.Arr[i] = Hash.Arr[i + 12];
	}
	delete [] Hash.Arr;

	return Address;
}
