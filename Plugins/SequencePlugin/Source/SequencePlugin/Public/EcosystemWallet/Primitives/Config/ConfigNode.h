#pragma once

class FConfigTopology;

class SEQUENCEPLUGIN_API FConfigNode 
{
public:
	FConfigNode(const TSharedPtr<FConfigTopology>& Left, const TSharedPtr<FConfigTopology>& Right)
	{
		this->Left = Left;
		this->Right = Right;
	}
	
	TSharedPtr<FConfigTopology> Left;
	TSharedPtr<FConfigTopology> Right;
};
