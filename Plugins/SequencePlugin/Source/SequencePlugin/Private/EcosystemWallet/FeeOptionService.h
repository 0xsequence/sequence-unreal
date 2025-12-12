#pragma once
#include "EcosystemWallet/Primitives/Calls/Call.h"
#include "Relayer/Models/FeeOption.h"

class SEQUENCEPLUGIN_API FFeeOptionService
{
public:
	explicit FFeeOptionService(const FFeeOption& FeeOption)
		: FeeOption(FeeOption) { }

	FCall BuildCall();

private:
	FCall BuildCallForNativeTokenOption();
	
	FCall BuildCallForCustomTokenOption();
	
	FFeeOption FeeOption;
};
