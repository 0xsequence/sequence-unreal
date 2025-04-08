#include "Integrators/SardineCheckoutBP.h"

void USardineCheckoutBP::CheckSardineWhiteListStatus(FString Address, FOnCheckSardineWhiteListStatus OnSuccess,
	FOnSardineCheckoutFailure OnFailure)
{
	SardineCheckout->CheckSardineWhiteListStatus(Address, [OnSuccess](bool Response)
	{
		if(OnSuccess.IsBound())
		{
			OnSuccess.Execute(Response);
		}
	}, [OnFailure](FSequenceError Error)
	{
		if(OnFailure.IsBound())
		{
			OnFailure.Execute(Error);
		}
	});
}
