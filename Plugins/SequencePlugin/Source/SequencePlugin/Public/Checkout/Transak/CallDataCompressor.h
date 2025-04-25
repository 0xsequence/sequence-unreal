#pragma once

#include "CoreMinimal.h"

/**
 * Utility class for compressing call data using Deflate + Base64 + URI encoding.
 */
class SEQUENCEPLUGIN_API FCallDataCompressor
{
public:
    static FString Compress(const FString& In);
};