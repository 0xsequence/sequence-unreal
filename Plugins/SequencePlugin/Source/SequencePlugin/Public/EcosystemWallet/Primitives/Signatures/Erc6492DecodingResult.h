#pragma once

class FErc6492;

struct SEQUENCEPLUGIN_API FErc6492DecodingResult
{
	TArray<uint8> Signature;
	TSharedPtr<FErc6492> Erc6492;
};
