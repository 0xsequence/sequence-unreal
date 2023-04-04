#include "Errors.h"

SequenceError::SequenceError(ErrorType Type, FString Message) : Message(Message), Type(Type)
{
}
