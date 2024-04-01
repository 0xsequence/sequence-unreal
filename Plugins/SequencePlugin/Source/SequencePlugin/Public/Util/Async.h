// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "Errors.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Interfaces/IHttpRequest.h"

template <typename T>
using TSuccessCallback =  TFunction<void (T)>;

template <typename T1, typename T2>
using TSuccessCallbackTuple =  TFunction<void (T1, T2)>;

using FFailureCallback =  TFunction<void (FSequenceError)>;