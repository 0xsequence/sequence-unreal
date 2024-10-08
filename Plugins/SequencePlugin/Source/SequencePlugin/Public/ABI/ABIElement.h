// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "GenericPlatform/GenericPlatform.h"

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
	virtual void Decode(TArray<uint8> &Data, int BlockPosition) = 0;
	static void PushEmptyBlock(TArray<uint8> &Data);

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
	TArray<uint8> Encode();
	virtual void EncodeHead(TArray<uint8> &Data) override;
	virtual void EncodeTail(TArray<uint8> &Data, int HeadPosition, int Offset) override;
	virtual TSharedPtr<ABIElement> Clone() override;
	virtual void Decode(TArray<uint8>& Data, int BlockPosition) override;
	void Push(TSharedPtr<ABIElement> Arg);
};

class SEQUENCEPLUGIN_API TDynamicABIArray : public ABIElement
{
	TArray<TSharedPtr<ABIElement>> MyData;
public:
	TDynamicABIArray();
	TDynamicABIArray(TArray<TSharedPtr<ABIElement>> MyData);
	virtual void EncodeHead(TArray<uint8> &Data) override;
	virtual void EncodeTail(TArray<uint8> &Data, int HeadPosition, int Offset) override;
	virtual TSharedPtr<ABIElement> Clone() override;
	virtual void Decode(TArray<uint8>& Data, int BlockPosition) override;
	void Push(TSharedPtr<ABIElement> Arg);
};

class SEQUENCEPLUGIN_API TFixedABIData : public ABIElement
{
	TArray<uint8> MyData;
public:
	TFixedABIData(TArray<uint8> MyData);
	virtual void EncodeHead(TArray<uint8> &Data) override;
	virtual void EncodeTail(TArray<uint8> &Data, int HeadPosition, int Offset) override;
	virtual TSharedPtr<ABIElement> Clone() override;
	virtual void Decode(TArray<uint8>& Data, int BlockPosition) override;
};

class SEQUENCEPLUGIN_API TDynamicABIData : public ABIElement
{
	TArray<uint8> MyData;
public:
	TDynamicABIData(TArray<uint8> MyData);
	virtual void EncodeHead(TArray<uint8> &Data) override;
	virtual void EncodeTail(TArray<uint8> &Data, int HeadPosition, int Offset) override;
	virtual TSharedPtr<ABIElement> Clone() override;
	virtual void Decode(TArray<uint8>& Data, int BlockPosition) override;
};