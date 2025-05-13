// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "Types/Block.h"

FBlock::~FBlock()
{
	delete Header;
}

FBlock MakeBlock(FJsonObject Json)
{
	return FBlock{
		
	};
}
