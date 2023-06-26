#pragma once
#include "Types/Types.h"
#include "Types/BinaryData.h"


class NumberCoder
{
public:
	static FString Encode(FHash256 num);
};