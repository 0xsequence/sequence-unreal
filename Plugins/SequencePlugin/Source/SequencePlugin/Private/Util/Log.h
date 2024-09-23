// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

DECLARE_LOG_CATEGORY_EXTERN(LogSequence, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSequenceEditor, Log, All);

#define SEQ_LOG(Verbosity, Format, ...) UE_LOG(LogSequence, Verbosity, Format, ##__VA_ARGS__)

#if WITH_EDITOR
#define SEQ_LOG_EDITOR(Verbosity, Format, ...) UE_LOG(LogSequenceEditor, Verbosity, Format, ##__VA_ARGS__)
#else
#define SEQ_LOG_EDITOR(Verbosity, Format, ...)  do {} while (0)
#endif
