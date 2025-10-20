#include "SignerService.h"

void FSignerService::FindSignersForCalls(TFunction<void(TArray<FSessionSigner>)> OnSuccess, TFunction<void(FString)> OnFailure)
{
	const FString IdentitySigner = this->SessionsTopology.Get()->GetIdentitySigner();
	const TArray<FString> ImplicitBlacklist = this->SessionsTopology.Get()->GetImplicitBlacklist();

	const TArray<FSessionSigner> ValidImplicitSigners = GetValidImplicitSigners(IdentitySigner, ImplicitBlacklist);
	const TArray<FSessionSigner> ValidExplicitSigners = GetValidExplicitSigners();

	TArray<FSessionSigner> AvailableSigners = ValidImplicitSigners;
	AvailableSigners.Append(ValidExplicitSigners);

	if (AvailableSigners.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No valid signers found."));
	}
	
	const TArray<FSessionSigner> SupportedSigners = FindSignerForEachCallAsync(ValidExplicitSigners, ValidImplicitSigners);
	if (this->Calls.Num() == SupportedSigners.Num())
	{
		OnSuccess(SupportedSigners);
	}
	else
	{
		OnFailure(FString::Printf(TEXT("No signers available for this call. %d %d"), this->Calls.Num(), SupportedSigners.Num()));
	}
}

TArray<FSessionSigner> FSignerService::GetValidImplicitSigners(const FString& IdentitySigner, const TArray<FString>& Blacklist)
{
	TArray<FSessionSigner> Result;

	for (FSessionSigner& Signer : this->Signers)
	{
		if (!Signer.Credentials.IsExplicit &&
			Signer.GetIdentitySigner().Equals(IdentitySigner, ESearchCase::IgnoreCase) &&
			!Blacklist.Contains(Signer.GetSessionAddress()))
		{
			Result.Add(Signer);
		}
	}

	return Result;
}

TArray<FSessionSigner> FSignerService::GetValidExplicitSigners()
{
	TArray<FSessionSigner> Result;
	TArray<FString> ExplicitSigners = this->SessionsTopology.Get()->GetExplicitSigners();

	for (const FSessionSigner& Signer : Signers)
	{
		if (Signer.Credentials.IsExplicit && ExplicitSigners.Contains(Signer.GetSessionAddress()))
		{
			Result.Add(Signer);
		}
	}

	return Result;
}

TArray<FSessionSigner> FSignerService::FindSignerForEachCallAsync(const TArray<FSessionSigner>& ExplicitSigners, const TArray<FSessionSigner>& ImplicitSigners)
{
	TArray<FSessionSigner> SupportedSigners;
	
	for (FSessionSigner ExplicitSigner : ExplicitSigners)
	{
		for (const FCall& Call : this->Calls)
		{
			if (ExplicitSigner.IsSupportedCall(ChainId, Call, SupportedSigners, SessionsTopology))
			{
				UE_LOG(LogTemp, Display, TEXT("Adding explicit signer %s"), *ExplicitSigner.GetSessionAddress())
				SupportedSigners.Add(ExplicitSigner);
			}
		}
	}

	for (FSessionSigner ImplicitSigner : ImplicitSigners)
	{
		for (const FCall& Call : this->Calls)
		{
			if (ImplicitSigner.IsSupportedCall(ChainId, Call, SupportedSigners, SessionsTopology))
			{
				UE_LOG(LogTemp, Display, TEXT("Adding implicit signer %s"), *ImplicitSigner.GetSessionAddress())
				SupportedSigners.Add(ImplicitSigner);
			}
		}
	}

	return SupportedSigners;
}
