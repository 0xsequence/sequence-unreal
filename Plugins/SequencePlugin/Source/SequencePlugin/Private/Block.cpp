#include "Block.h"
#include "Json.h"

FBlock::~FBlock()
{
	delete Header;
}

FBlock MakeBlock(FJsonObject Json)
{
	return FBlock{
		
	};
}
