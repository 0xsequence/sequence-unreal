// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "Errors.h"

FSequenceError::FSequenceError(const EErrorType Type, const FString& Message) :
	Response(nullptr), Message(Message), Type(Type) { }

FSequenceError::FSequenceError(const EErrorType Type, const FHttpResponsePtr& Response, const FString& Message) :
	Response(Response), Message(Message), Type(Type) { }
