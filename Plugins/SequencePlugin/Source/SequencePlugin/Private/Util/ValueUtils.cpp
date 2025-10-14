#include "Util/ValueUtils.h"

FString FValueUtils::Int32ToString(const int32& InValue)
{
	return FString::Printf(TEXT("%i"), InValue);
}

int32 FValueUtils::StringToInt32(const FString& InValue)
{
	return FCString::Atoi(*InValue);
}
