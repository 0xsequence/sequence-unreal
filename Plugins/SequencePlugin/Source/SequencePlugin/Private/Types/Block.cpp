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
