#include "Errors.h"

FSequenceError::FSequenceError(const EErrorType Type, const FString Message) : Message(Message), Type(Type)
{
}
