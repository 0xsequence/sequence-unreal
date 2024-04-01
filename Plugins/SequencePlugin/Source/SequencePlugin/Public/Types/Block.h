// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "Header.h"

struct FBlock
{
	FHeader* Header;
	FHeader* Uncles;
	uint8 HeaderNum;
	~FBlock();
};

FBlock MakeBlock(FJsonObject Json);