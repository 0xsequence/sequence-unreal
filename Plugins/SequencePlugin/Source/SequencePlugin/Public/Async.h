#pragma once

#include "Errors.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Interfaces/IHttpRequest.h"

template <typename T>
using TSuccessCallback =  TFunction<void (T)>;
using TFailureCallback =  TFunction<void (SequenceError)>;

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

template <typename TIn, typename  TOut>
class Chain
{
	TFailureCallback OnFailure;
	TFunction<TResult<TOut> (TIn)> Function;

public:
	Chain(TFunction<TResult<TOut> (TIn)> Function, TFailureCallback OnFailure);
	
	template <typename TNext>
	Chain<TIn, TNext> Then(TFunction<TNext (TOut)> NextFunction);

	TFunction<TResult<TOut> (TIn)> Get();
};

template <typename TIn, typename TOut>
Chain<TIn, TOut>::Chain(TFunction<TResult<TOut> (TIn)> Function, TFailureCallback OnFailure) : OnFailure(OnFailure), Function(Function)
{
}

template <typename TIn, typename TOut>
template <typename TNext>
Chain<TIn, TNext> Chain<TIn, TOut>::Then(TFunction<TNext(TOut)> NextFunction)
{
	TFunction<TNext (TIn)> ChainedFunctions = [this, &NextFunction](TIn Input)
	{
		TResult<TOut> Result = this->Function(Input);
		TResult<TNext> RetVal;

		if(!Result.HasError())
		{
			RetVal = MakeValue(NextFunction(Result.GetValue()));
		}
		else
		{
			RetVal = MakeError(Result.GetError());
		}

		return RetVal;
	};
	
	return Chain<TIn, TNext>(ChainedFunctions, this->OnFailure);
}

template <typename TIn, typename TOut>
TFunction<TResult<TOut>(TIn)> Chain<TIn, TOut>::Get()
{
	return this->Function;
}
