// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "Errors.h"

FSequenceError::FSequenceError(const EErrorType Type, const FString Message) : Message(Message), Type(Type)
{
}
