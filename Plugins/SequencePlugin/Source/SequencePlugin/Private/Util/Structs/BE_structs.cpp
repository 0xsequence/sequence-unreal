#include "Util/Structs/BE_Structs.h"
#include "Sequence/SequenceAPI.h"

FString FTransaction_FE::ID() const
{
	return SequenceAPI::FTransaction_Sequence::Convert(*this).ID();
}
