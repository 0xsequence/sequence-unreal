#pragma once

#include "CoreMinimal.h"

class FSessionsTopology;

class SEQUENCEPLUGIN_API FSessionsBranch
{
public:
	explicit FSessionsBranch(const TArray<TSharedPtr<FSessionsTopology>>& InChildren)
	: Children(InChildren) { }

	explicit FSessionsBranch(TArray<TSharedPtr<FSessionsTopology>>&& InChildren)
	: Children(MoveTemp(InChildren)) { }
	
	TArray<TSharedPtr<FSessionsTopology>> Children;
};
