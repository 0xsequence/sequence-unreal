#include "Checkout/Transak/CallDataCompressor.h"
#include "Misc/Base64.h"
#include "Misc/Compression.h"
#include "GenericPlatform/GenericPlatformHttp.h" 

FString FCallDataCompressor::Compress(const FString& In)
{
	FTCHARToUTF8 Converter(*In);
	const uint8* UncompressedData = reinterpret_cast<const uint8*>(Converter.Get());
	int32 UncompressedSize = Converter.Length();

	int32 CompressedBufferSize = FCompression::CompressMemoryBound(NAME_Zlib, UncompressedSize);
	TArray<uint8> CompressedData;
	CompressedData.SetNumUninitialized(CompressedBufferSize);

	int32 CompressedSize = CompressedBufferSize;
	bool bSuccess = FCompression::CompressMemory(
		NAME_Zlib,
		CompressedData.GetData(),
		CompressedSize,
		UncompressedData,
		UncompressedSize,
		COMPRESS_Default
	);

	if (!bSuccess)
	{
		UE_LOG(LogTemp, Error, TEXT("CallData compression failed"));
		return TEXT("CompressionError");
	}

	CompressedData.SetNum(CompressedSize);
	FString Base64 = FBase64::Encode(CompressedData);
	return FGenericPlatformHttp::UrlEncode(Base64);
}
