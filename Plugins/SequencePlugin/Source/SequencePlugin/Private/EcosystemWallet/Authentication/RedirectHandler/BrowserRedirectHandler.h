#pragma once

#include "IRedirectHandler.h"

class SEQUENCEPLUGIN_API FBrowserRedirectHandler : public IRedirectHandler
{
public:
	virtual void WaitForResponseImpl(const FString& FullUrl, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure) override;
};
