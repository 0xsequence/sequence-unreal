#pragma once

#include "GenericPlatform/GenericPlatformMisc.h"

template <typename T>
TFuture<void> Pipe(TFuture<T>& Input, TFunction<void (T)>& CallBack)
{
	return Async(EAsyncExecution::Thread, [&Input, &CallBack]()
	{
		CallBack(Input.Get());
	});
}
