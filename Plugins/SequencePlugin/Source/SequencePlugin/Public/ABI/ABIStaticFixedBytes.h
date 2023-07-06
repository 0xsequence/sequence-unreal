#pragma once
#include "Types/BinaryData.h"
#include "ABI/ABI.h"
#include "ABI/ABITypes.h"

template<uint32 Size>
class FABIStaticFixedBytes : public TABIPropertyWithValue<FUniformData<Size>>
{
public:
	FABIStaticFixedBytes();
	FABIStaticFixedBytes(FUniformData<Size> initialValue);
	virtual FABIArg Serialize() override;
	virtual FABIArg BlankArg() override;
	virtual void Deserialize(FABIArg Arg) override;
};

template <uint32 Size>
FABIStaticFixedBytes<Size>::FABIStaticFixedBytes() : TABIPropertyWithValue<FUniformData<Size>>(FUniformData<Size>{nullptr})
{
}

template <uint32 Size>
FABIStaticFixedBytes<Size>::FABIStaticFixedBytes(FUniformData<Size> initialValue): TABIPropertyWithValue<FUniformData<Size>>(initialValue)
{
}

template <uint32 Size>
FABIArg FABIStaticFixedBytes<Size>::Serialize()
{
	auto Copy = this->GetValue().Copy();

	return FABIArg{STATIC, Size, Copy.Arr};
}

template <uint32 Size>
FABIArg FABIStaticFixedBytes<Size>::BlankArg()
{
	return FABIArg{STATIC, Size, nullptr};
}

template <uint32 Size>
void FABIStaticFixedBytes<Size>::Deserialize(FABIArg Arg)
{
	auto ArgData = static_cast<uint8*>(Arg.Data);
	auto Data = new uint8[Size];

	for(auto i = 0; i < Size; i++)
	{
		Data[i] = ArgData[i];
	}
	
	this->SetValue(FUniformData<Size>{Data});
	Arg.Destroy();
}



