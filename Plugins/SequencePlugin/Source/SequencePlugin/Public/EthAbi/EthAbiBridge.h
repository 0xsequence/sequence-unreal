#pragma once

#include "Misc/Base64.h"

class FEthAbiBridge
{
public:
	/**
	 * Encode function parameters based on the function signature.
	 * @param FunctionSignature such as balanceOf(address,uint256)
	 * @param Values JSON array such as ["0x6615e4e985bf0d137196897dfa182dbd7127f54f", 2]
	 * @return Encoded hex string used to send as transaction data
	 */
	static FString EncodeFunctionCall(const FString& FunctionSignature, const FString& Values);

	/**
	 * The decoded data represents the ABI structure. In this case, it's an array of values in the array of 'outputs'
	 * Example: [[10000,0,"0x0000000000000000000000000000000000000000000000000000000000000000",1747809900,1905662700,"0x0000000000000000000000000000000000000000000000000000000000000000"]]
	 * @param Abi Function ABI in the following format: {"type": "function", ...}
	 * @param EncodedData The hex string you receive from a Call() function.
	 * @return Decoded data as an unnamed JSON array of values.
	 */
	static FString DecodeFunctionResult(const FString& Abi, const FString& EncodedData);

	/**
	 * Encode the given big integer into a byte array.
	 * @param Value The BigInteger value as a string type.
	 * @return The encoded byte array.
	 */
	static TArray<uint8> EncodeBigInteger(const FString& Value);

	/**
	 * Convert a hex such as '0x0000000000000000000000000000000000000000000000000000000000000020' to a Big Int '32'
	 * @param Hex The encoded BigInt as a hex
	 * @return The converted BigInt as a string
	 */
	static FString HexToBigIntString(const FString& Hex);

	static TArray<uint8> EncodeAbiPacked(const FString& JsonInput);
	
	static TArray<FString> DecodeAbiJson(const TArray<uint8>& Raw, const FString& TypesJson);
};
