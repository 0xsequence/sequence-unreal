#pragma once

using ByteLength = uint32;
using Hash = uint8*;

constexpr ByteLength GHash256ByteLength = 32;
using Hash256 = Hash; // 256 bit hash, 32 bytes

constexpr ByteLength GAddressByteLength = 20;
using Address = Hash; // 20 byte address

constexpr ByteLength GPublicKeyByteLength = 64;
using PublicKey = Hash; // 64 byte key

constexpr ByteLength GPrivateKeyByteLength = 32;
using PrivateKey = Hash; // 32 byte key

constexpr ByteLength GBloomByteLength = 256;
using Bloom = Hash; // 256 bytes, 2048 bits

constexpr ByteLength GBlockNonceByteLength = 8;
using BlockNonce = Hash; // 8 bytes, 64 bits

// Number of arbitrary length
// Must be deallocated
struct FBinaryData
{
	uint8* Data;
	uint8 ByteLength;
};



