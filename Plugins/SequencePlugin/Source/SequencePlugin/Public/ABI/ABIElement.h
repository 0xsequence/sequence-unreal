#pragma once
#include "CoreMinimal.h"
#include "GenericPlatform/GenericPlatform.h"
#include "Types/BinaryData.h"

class SEQUENCEPLUGIN_API ABIElement
{
public:
	virtual ~ABIElement() = default;
	virtual void EncodeHead(TArray<uint8> &Data) = 0;
	/*
	 * The Headposition is how far back the head of the entire array of arguments is,
	 *  while the offset is the argument number 
	 */
	virtual void EncodeTail(TArray<uint8> &Data, int HeadPosition, int Offset) = 0;
	virtual TSharedPtr<ABIElement> Clone() = 0; // Used to fill out arrays during decoding
	virtual void Decode(TArray<uint8> &Data, int BlockPosition, int HeadPosition) = 0;
	TArray<TSharedPtr<ABIElement>> MyData;
	static void PushEmptyBlock(TArray<uint8> &Data);

	// Decoding Helpers
	virtual TArray<TSharedPtr<ABIElement>> AsArray();
	virtual TArray<uint8> AsRawBinary();
	virtual FUnsizedData AsUnsizedBinary();
	virtual uint32 AsUInt32();
	virtual int32 AsInt32();
	virtual bool AsBool();
	virtual FString AsString();
	virtual FAddress AsAddress();

	// Integer Utilities
	static void CopyInUInt32(TArray<uint8> &Data, uint32 Value, int BlockPosition);
	static void CopyInInt32(TArray<uint8> &Data, int32 Value, int BlockPosition);
	static uint32 CopyOutUInt32(TArray<uint8> &Data, int BlockPosition);
	static int32 CopyOutInt32(TArray<uint8> &Data, int BlockPosition);
};

class SEQUENCEPLUGIN_API TFixedABIArray : public ABIElement
{
	TArray<TSharedPtr<ABIElement>> MyData;
public:
	TFixedABIArray();
	TFixedABIArray(TArray<TSharedPtr<ABIElement>> MyData);

	virtual TArray<TSharedPtr<ABIElement>> AsArray() override;
	
	TArray<uint8> Encode();
	virtual void EncodeHead(TArray<uint8> &Data) override;
	virtual void EncodeTail(TArray<uint8> &Data, int HeadPosition, int Offset) override;
	virtual TSharedPtr<ABIElement> Clone() override;
	
	// Fixed Arrays must be fully populated with members before decoding
	// E.g. a fixed Array<int, 2> must contain two TFixedAbiData members, one for each int
	virtual void Decode(TArray<uint8>& Data, int BlockPosition, int HeadPosition) override;

	void Push(TSharedPtr<ABIElement> Arg);
};

class SEQUENCEPLUGIN_API TDynamicABIArray : public ABIElement
{
	TArray<TSharedPtr<ABIElement>> MyData;
public:
	TDynamicABIArray();
	TDynamicABIArray(TArray<TSharedPtr<ABIElement>> MyData);

	virtual TArray<TSharedPtr<ABIElement>> AsArray() override;
	
	virtual void EncodeHead(TArray<uint8> &Data) override;
	virtual void EncodeTail(TArray<uint8> &Data, int HeadPosition, int Offset) override;
	virtual TSharedPtr<ABIElement> Clone() override;
	virtual void Decode(TArray<uint8>& Data, int BlockPosition, int HeadPosition) override;
	void Push(TSharedPtr<ABIElement> Arg);
};

class SEQUENCEPLUGIN_API TFixedABIData : public ABIElement
{
	TArray<uint8> MyData;
public:
	TFixedABIData(TArray<uint8> MyData);
	TFixedABIData(int Size);

	virtual TArray<uint8> AsRawBinary() override;
	virtual uint32 AsUInt32() override;
	virtual int32 AsInt32() override;
	virtual bool AsBool() override;
	
	virtual void EncodeHead(TArray<uint8> &Data) override;
	virtual void EncodeTail(TArray<uint8> &Data, int HeadPosition, int Offset) override;
	virtual TSharedPtr<ABIElement> Clone() override;
	virtual void Decode(TArray<uint8>& Data, int BlockPosition, int HeadPosition) override;
};

class SEQUENCEPLUGIN_API TDynamicABIData : public ABIElement
{
	TArray<uint8> MyData;
public:
	TDynamicABIData(TArray<uint8> MyData);

	virtual TArray<uint8> AsRawBinary() override;
	virtual FString AsString() override;
	virtual FAddress AsAddress() override;
	
	virtual void EncodeHead(TArray<uint8> &Data) override;
	virtual void EncodeTail(TArray<uint8> &Data, int HeadPosition, int Offset) override;
	virtual TSharedPtr<ABIElement> Clone() override;
	virtual void Decode(TArray<uint8>& Data, int BlockPosition, int HeadPosition) override;
};