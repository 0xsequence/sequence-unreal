#pragma once

class FConfigTopology;

class SEQUENCEPLUGIN_API FConfigNode 
{
public:
	FConfigNode(const TSharedPtr<FConfigTopology>& Left, const TSharedPtr<FConfigTopology>& Right)
	{
		this->Left = Left.Get();
		this->Right = Right.Get();
	}
	
	const FConfigTopology* Left;
	const FConfigTopology* Right;
};
