#include "SessionsTopology.h"

TSharedPtr<FSessionsLeaf> FSessionsTopology::FindIdentitySigner(const FString& Address) const
{
	return MakeShareable(new FSessionsLeaf);
}

TSharedPtr<FSessionsTopology> FSessionsTopology::FromServiceConfigTree(const TSharedPtr<FJsonValue>& Input)
{
	if (Input->Type == EJson::Array)
	{
		TArray<TSharedPtr<FJsonValue>> List = Input->AsArray();
		if (List.Num() < 2)
		{
			return MakeShareable(new FSessionsTopology());
		}

		TArray<TSharedPtr<FSessionsTopology>> Children = TArray<TSharedPtr<FSessionsTopology>>();
		for (int32 i = 0; i < List.Num(); i++)
		{
			Children.Add(FromServiceConfigTree(List[i]));
		}
        
		return MakeShared<FSessionsTopology>(FSessionsTopology(MakeShared<FSessionsBranch>(Children)));
	}

	if (Input->Type != EJson::Object)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid JSON object"));
		return MakeShared<FSessionsTopology>(FSessionsTopology());
	}
	
	return MakeShareable(new FSessionsTopology());
}
