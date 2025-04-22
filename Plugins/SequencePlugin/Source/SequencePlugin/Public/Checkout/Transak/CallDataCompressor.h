#pragma once

#include "CoreMinimal.h"

namespace CallDataCompressor
{
	// Compresses call data using Deflate + Base64 + URI escape
	SEQUENCEPLUGIN_API FString Compress(const FString& In);
}