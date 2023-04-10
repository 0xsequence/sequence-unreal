#pragma once

using Size = uint8;
using Hash = uint8*;

constexpr Size GHash256ByteLength = 32;
using Hash256 = Hash; // 256 bit hash, 32 bytes
constexpr Size GAddressByteLength = 20;
using Address = Hash; // 20 byte address

constexpr Size GBloomByteLength = 256;
using Bloom = Hash; // 256 bytes, 2048 bits

constexpr Size GBlockNonceByteLength = 8;
using BlockNonce = Hash; // 8 bytes, 64 bits

// Number of arbitrary length
// Must be deallocated
struct FBigNum
{
	uint8* Data;
	uint8 ByteLength;
};



