#pragma once

#include "CoreMinimal.h"
#include "ExtensionsData.h"

class SEQUENCEPLUGIN_API FExtensionsFactory
{
public:
	inline static const FExtensionsData Rc3 {
		TEXT("0x00000000000018A77519fcCCa060c2537c9D6d3F"), // Factory
		TEXT("0x00000000000084fA81809Dd337311297C5594d62"), // Stage1
		TEXT("0x7438718F9E4b9B834e305A620EEeCf2B9E6eBE79"), // Stage2
		TEXT("0x6041600e3d396021805130553df33d3d36153402601f57363d3d373d363d30545af43d82803e903d91601f57fd5bf3"), // CreationCode
		TEXT("0x0000000000CC58810c33F3a0D78aA1Ed80FaDcD8"), // Sessions
		TEXT("0x18002Fc09deF9A47437cc64e270843dE094f5984") 
	};

	static const FExtensionsData& GetCurrent()
	{
		return Rc3;
	}
};