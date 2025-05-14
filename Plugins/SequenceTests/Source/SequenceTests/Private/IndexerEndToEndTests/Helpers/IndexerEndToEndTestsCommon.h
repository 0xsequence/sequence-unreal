#pragma once

#include "CoreMinimal.h"

namespace IndexerEndToEndTestsCommon
{
	const FString TestAddress = TEXT("0x8e3E38fe7367dd3b52D1e281E4e8400447C8d8B9"); // From the Sequence Docs - this address has a fair amount of tokens held on Polygon and works great for our test cases
	const int64 PolygonNetworkId = 137;
	const FString TestContractAddress_Skyweaver = TEXT("0x631998e91476DA5B870D741192fc5Cbc55F5a52E");
	const FString TestContractAddress_USDC = TEXT("0x3c499c542cEF5E3811e1192ce70d8cC03d5c3359");
	const FString TestContractAddress_WMATIC = TEXT("0x0d500B1d8E8eF31E21C99d1Db9A6444d3ADf1270");
}