#pragma once
#include "Types/BinaryData.h"

using FImageHash = FUnsizedData;

class FImageHashable
{
public:
	virtual ~FImageHashable() = default;
	virtual FImageHash ImageHash() = 0;
};

class FSignature
{
	
};

class FWalletConfig : public FImageHashable
{
public:
	virtual FImageHash ImageHash() override;
};

class FSubDigest
{
public:
	
};