#pragma once

#include "GenericPlatform/GenericPlatformMisc.h"

template <typename T>
void PipeData(TFunction<T> Input, TFunction<void (T)> CallBack)
{
	TFuture<T> Data = Async(EAsyncExecution::Thread, Input);
	CallBack(Data.Get());
}
/*
template <typename T>
void PipeDataAsync(TFunction<T> Input, TFunction<void (T)> CallBack)
{
	Async(EAsyncExecution::Thread, [Input, CallBack](){ PipeData(Input, CallBack); });
}*/