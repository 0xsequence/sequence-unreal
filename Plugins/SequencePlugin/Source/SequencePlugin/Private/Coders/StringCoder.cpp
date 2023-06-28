#include "Coders/StringCoder.h"
#include <iostream>
#include <Coders/FixedByteCoder.h>

FString StringCoder::Encode(FString value)
{
//count bits in string
//use fixed bytecoder
	char hex_length[20];
	auto len = value.Len(); //2^32 max len of string
	sprintf(hex_length, "%X", len);
	auto hexLen = FString(hex_length);
	UE_LOG(LogTemp, Display, TEXT("hex len: %s"), *hexLen);
	auto encodedLen = FString::ChrN(64-hexLen.Len(),'0') + hexLen;
	UE_LOG(LogTemp, Display, TEXT("Encoded Len: %s"), *encodedLen);
	auto mFixedByteCoder = FixedByteCoder();
	auto encodedString = mFixedByteCoder.Encode(value);
	UE_LOG(LogTemp, Display, TEXT("Encoded String: %s"), *encodedString);
	UE_LOG(LogTemp, Display, TEXT("Final encoding: %s"), *(encodedLen+encodedString));
	return encodedLen+encodedString;
}
