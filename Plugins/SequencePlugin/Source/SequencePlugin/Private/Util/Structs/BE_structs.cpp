// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "Util/Structs/BE_Structs.h"
#include "Sequence/SequenceAPI.h"

FString FTransaction_FE::ID() const
{
	return FTransaction_Sequence::Convert(*this).ID();
}
