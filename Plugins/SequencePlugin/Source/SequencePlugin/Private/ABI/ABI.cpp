// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "ABI/ABI.h"
#include "ABI/ABIElement.h"
#include "Eth/Crypto.h"
#include "Types/BinaryData.h"

TFixedABIData ABI::UInt32(uint32 Input)
{
	TArray<uint8> Arr;
	ABIElement::PushEmptyBlock(Arr);
	ABIElement::CopyInUInt32(Arr, Input, 0);
	return TFixedABIData(Arr);
}

TFixedABIData ABI::Int32(int32 Input)
{
	TArray<uint8> Arr;
	ABIElement::PushEmptyBlock(Arr);
	ABIElement::CopyInInt32(Arr, Input, 0);
	return TFixedABIData(Arr);
}

TFixedABIData ABI::Bool(bool Input)
{
	TArray<uint8> Arr;
	ABIElement::PushEmptyBlock(Arr);
	Arr[GBlockByteLength - 1] = Input ? 0x01 : 0x00;
	return TFixedABIData(Arr);
}

TFixedABIData ABI::Address(FAddress Address)
{
	TArray<uint8> Arr = Address.ToArray();

	// Left padded
	for(auto i = 0; i < GBlockByteLength - FAddress::Size; i++)
	{
		Arr.Insert(0x00, 0);
	}

	return TFixedABIData{Arr};
}

TDynamicABIData ABI::String(FString Input)
{
	TArray<uint8> Arr = StringToUTF8(Input).ToArray();
	return TDynamicABIData(Arr);
}


// DEPRECATED
FUnsizedData ABI::Encode(FString Signature, TArray<ABIElement*> Args)
{
	TArray<TSharedPtr<ABIElement>> NewArgs;
	for(auto i = 0; i < Args.Num(); i++)
	{
		NewArgs.Push(Args[i]->Clone());
	}

	TOptional<FUnsizedData> Encoded = Encode(Signature, NewArgs);

	if(Encoded.IsSet())
	{
		return Encoded.GetValue();
	}
	else
	{
		return FUnsizedData { TArray<uint8>() };
	}
}

TOptional<FUnsizedData> ABI::Encode(FString Signature, TArray<TSharedPtr<ABIElement>> Args)
{
	if(!IsValidSignature(Signature))
	{
		UE_LOG(LogTemp, Display, TEXT("Invalid signature! Remember that ABI signatures should only contain argument types without spaces and NOT names."));
		return TOptional<FUnsizedData>();
	}
	
	TFixedABIArray FixedArr(Args);
	FUnsizedData Data {
		FixedArr.Encode()
	};
	FUnsizedData SignatureData = StringToUTF8(Signature);
	FHash256 Hash = GetKeccakHash(SignatureData);
	Data.Arr->Insert(Hash.Ptr(), GSignatureLength, 0);
	return Data;
}

void ABI::Decode(TArray<uint8> Data, TArray<TSharedPtr<ABIElement>> Args)
{
	TFixedABIArray FixedArr(Args);
	FixedArr.Decode(Data, GSignatureLength, GSignatureLength);
}

bool ABI::IsValidSignature(FString Signature)
{
	const FRegexPattern FunctionABIRegex(TEXT("^[A-Z|a-z|,-|0-9]+(([A-Z|a-z|0-9]+(, [A-Z|a-z|0-9]+))?)$"));
	FRegexMatcher MyMatcher(FunctionABIRegex, Signature);
	return MyMatcher.FindNext();
}

FString ABI::Display(FString Signature, TArray<TSharedPtr<ABIElement>> Args)
{
	FString Str = "";
	TOptional<FUnsizedData> Encoded = Encode(Signature, Args);

	if(!Encoded.IsSet())
	{
		return "Signature not valid!";
	}
	
	FString Reference = Encoded.GetValue().ToHex();

	for(int i = 0; i < Reference.Len(); i++)
	{
		if(i % (GBlockByteLength * 2) == GSignatureLength * 2)
		{
			Str += '\n';
		}
		
		Str += Reference[i];
	}
	
	return Str;
}
