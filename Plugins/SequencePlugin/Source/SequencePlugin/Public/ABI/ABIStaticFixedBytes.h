#pragma once
#include "Types/BinaryData.h"
#include "ABI/ABI.h"
#include "ABI/ABITypes.h"

template<uint32 Size>
class FABIFixedBytes : public TABIPropertyWithValue<FUniformData<Size>>
{
public:
	FABIFixedBytes();
	FABIFixedBytes(FUniformData<Size> initialValue);
	virtual FABIArg Serialize() override;
	virtual FABIArg BlankArg() override;
	virtual void Deserialize(FABIArg Arg) override;
};

template <uint32 Size>
FABIFixedBytes<Size>::FABIFixedBytes() : TABIPropertyWithValue<FUniformData<Size>>(FUniformData<Size>{nullptr})
{
}

template <uint32 Size>
FABIFixedBytes<Size>::FABIFixedBytes(FUniformData<Size> initialValue): TABIPropertyWithValue<FUniformData<Size>>(initialValue)
{
}

template <uint32 Size>
FABIArg FABIFixedBytes<Size>::Serialize()
{
	auto Copy = this->GetValue().Copy();

	return FABIArg{STATIC, Size, Copy.Arr};
}

template <uint32 Size>
FABIArg FABIFixedBytes<Size>::BlankArg()
{
	return FABIArg{STATIC, Size, nullptr};
}

template <uint32 Size>
void FABIFixedBytes<Size>::Deserialize(FABIArg Arg)
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



