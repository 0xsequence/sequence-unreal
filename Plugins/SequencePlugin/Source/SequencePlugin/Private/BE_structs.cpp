#include "BE_Structs.h"
#include "Sequence/SequenceAPI.h"

FString FTransaction_FE::ID() const
{
	return SequenceAPI::FTransaction::Convert(*this).ID();
}
