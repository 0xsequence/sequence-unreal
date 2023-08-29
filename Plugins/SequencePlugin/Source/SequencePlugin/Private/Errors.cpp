#include "Errors.h"

FSequenceError::FSequenceError(EErrorType Type, FString Message) : Message(Message), Type(Type)
{
}
