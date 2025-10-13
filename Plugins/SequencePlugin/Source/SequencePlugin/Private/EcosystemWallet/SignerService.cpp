#include "SignerService.h"

void FSignerService::FindSignersForCalls(TFunction<void(TArray<FSessionSigner>)> OnSuccess, TFunction<void()> OnFailure)
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

	FindSignerForEachCallAsync(AvailableSigners, [this, OnSuccess, OnFailure](TArray<FSessionSigner> SupportedSigners)
	{
		if (this->Calls.Num() == SupportedSigners.Num())
		{
			OnSuccess(SupportedSigners);
		}
		else
		{
			OnFailure();
		}
	});
}

TArray<FSessionSigner> FSignerService::GetValidImplicitSigners(const FString& IdentitySigner, const TArray<FString>& Blacklist)
{
	TArray<FSessionSigner> Result;

	for (FSessionSigner& Signer : this->Signers)
	{
		if (!Signer.Credentials.IsExplicit &&
			Signer.GetIdentitySigner().Equals(IdentitySigner, ESearchCase::IgnoreCase) &&
			!Blacklist.Contains(Signer.Credentials.SessionAddress))
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
		if (Signer.Credentials.IsExplicit && ExplicitSigners.Contains(Signer.Credentials.SessionAddress))
		{
			Result.Add(Signer);
		}
	}

	return Result;
}

void FSignerService::FindSignerForEachCallAsync(const TArray<FSessionSigner>& AvailableSigners, const TFunction<void(TArray<FSessionSigner>)>& OnCompleted)
{
	TSharedRef<TArray<FSessionSigner>, ESPMode::ThreadSafe> Signers = MakeShared<TArray<FSessionSigner>, ESPMode::ThreadSafe>();
	TSharedRef<int32, ESPMode::ThreadSafe> PendingCount = MakeShared<int32, ESPMode::ThreadSafe>(0);

	for (const FCall& Call : this->Calls)
	{
		for (FSessionSigner Signer : AvailableSigners)
		{
			(*PendingCount)++;
			
			Signer.IsSupportedCall(Call, [Signer, Signers, PendingCount, OnCompleted](const bool Supported)
			{
				if (Supported)
				{
					Signers->Add(Signer);
				}

				(*PendingCount)--;

				if (*PendingCount == 0)
				{
					OnCompleted(*Signers);
				}
			});
		}
	}
}
