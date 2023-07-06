#pragma once
#include "Types/BinaryData.h"
#include "ABI/ABI.h"
#include "ABI/ABITypes.h"

template<uint32 Size>
class FABIStaticFixedBytesProperty : public TABIPropertyWithValue<FUniformData<Size>>
{
public:
	FABIStaticFixedBytesProperty();
	FABIStaticFixedBytesProperty(FUniformData<Size> initialValue);
	virtual FABIArg Serialize() override;
	virtual FABIArg BlankArg() override;
	virtual void Deserialize(FABIArg Arg) override;
};

template <uint32 Size>
FABIStaticFixedBytesProperty<Size>::FABIStaticFixedBytesProperty() : TABIPropertyWithValue<FUniformData<Size>>(FUniformData<Size>{nullptr})
{
}

template <uint32 Size>
FABIStaticFixedBytesProperty<Size>::FABIStaticFixedBytesProperty(FUniformData<Size> initialValue): TABIPropertyWithValue<FUniformData<Size>>(initialValue)
{
}

template <uint32 Size>
FABIArg FABIStaticFixedBytesProperty<Size>::Serialize()
{
	auto Copy = this->GetValue().Copy();

	return FABIArg{STATIC, Size, Copy.Arr};
}

template <uint32 Size>
FABIArg FABIStaticFixedBytesProperty<Size>::BlankArg()
{
	return FABIArg{STATIC, Size, nullptr};
}

template <uint32 Size>
void FABIStaticFixedBytesProperty<Size>::Deserialize(FABIArg Arg)
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



