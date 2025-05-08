
#pragma once
#include "Header.h"

struct SEQUENCEPLUGIN_API FBlock
{
	FHeader* Header;
	FHeader* Uncles;
	uint8 HeaderNum;
	~FBlock();
};

FBlock SEQUENCEPLUGIN_API MakeBlock(FJsonObject Json);