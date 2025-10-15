#include "FeeOptionService.h"
#include "EthAbi/EthAbiBridge.h"
#include "Util/ValueUtils.h"

FCall FFeeOptionService::BuildCall()
{
	if (FeeOption.Token.Type == "ERC20_TOKEN")
		return BuildCallForCustomTokenOption();

	return BuildCallForNativeTokenOption();
}

FCall FFeeOptionService::BuildCallForNativeTokenOption()
{
	const FString ValueForwarder = "0xABAAd93EeE2a569cF0632f39B10A9f5D734777ca";
	const FString Values = FString::Printf(TEXT("[\"%s\",%s]"), *FeeOption.To, *FeeOption.Value);
	const FString EncodedFeeOptionData = FEthAbiBridge::EncodeFunctionCall("forwardValue(address,uint256)", Values);

	return FCall(
		ValueForwarder,
		EncodedFeeOptionData,
		FBigInt(FeeOption.Value),
		FBigInt(FValueUtils::Int32ToString(FeeOption.GasLimit)),
		false, false, "1");
}

FCall FFeeOptionService::BuildCallForCustomTokenOption()
{
	const FString Values = FString::Printf(TEXT("[\"%s\",%s]"), *FeeOption.To, *FeeOption.Value);
	const FString EncodedFeeOptionData = FEthAbiBridge::EncodeFunctionCall("transfer(address,uint256)", Values);

	return FCall(
		FeeOption.Token.ContractAddress,
		EncodedFeeOptionData,
		FBigInt("0"),
		FBigInt(FValueUtils::Int32ToString(FeeOption.GasLimit)),
		false, false, "1");
}
