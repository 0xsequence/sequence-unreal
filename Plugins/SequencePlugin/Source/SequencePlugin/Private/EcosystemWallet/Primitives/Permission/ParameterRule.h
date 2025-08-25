#pragma once

class SEQUENCEPLUGIN_API ParameterRule
{
public:
	bool Cumulative;
	int32 Operation;
	int64 Offset;
	TArray<uint8> Value;
	TArray<uint8> Mask;
};
