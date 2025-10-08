#pragma once

class FSessionsTopology;

class SEQUENCEPLUGIN_API FSessionsBranch
{
public:
	explicit FSessionsBranch(const TArray<TSharedPtr<FSessionsTopology>>& InChildren)
	{
		this->Children = InChildren;
	}
	
	TArray<TSharedPtr<FSessionsTopology>> Children;
};
