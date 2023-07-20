#pragma once

#include "Errors.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Interfaces/IHttpRequest.h"

template <typename T>
using SuccessCallback =  TFunction<void (T)>;
using FailureCallback =  TFunction<void (SequenceError)>;

template <typename T>
TFuture<void> SendAsync(TFuture<T>& Input, TFunction<void (T)>& CallBack)
{
	return Async(EAsyncExecution::Thread, [&Input, &CallBack]()
	{
		CallBack(Input.Get());
	});
}

template <typename TIn, typename TOut>
TFuture<TOut> PipeAsync(TFuture<TIn>& Input, TFunction<TOut (TIn)>& TransformFunction)
{
	return Async(EAsyncExecution::Thread, [&Input, &TransformFunction]
	{
		return TransformFunction(Input.Get());
	});
}