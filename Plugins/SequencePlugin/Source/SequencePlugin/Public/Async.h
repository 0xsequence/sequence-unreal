#pragma once

#include "GenericPlatform/GenericPlatformMisc.h"

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
