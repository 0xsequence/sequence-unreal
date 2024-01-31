#pragma once
#include "Types/BinaryData.h"
#include "ABI/ABI.h"
#include "ABI/ABITypes.h"

template<uint32 Size>
class FABIStaticFixedBytesProperty : public TABIPropertyWithValue<TSizedData<Size>>
{
public:
	FABIStaticFixedBytesProperty();
	FABIStaticFixedBytesProperty(TSizedData<Size> initialValue);
	virtual FABIArg Serialize() override;
	virtual FABIArg BlankArg() override;
	virtual void Deserialize(FABIArg Arg) override;
};

template <uint32 Size>
FABIStaticFixedBytesProperty<Size>::FABIStaticFixedBytesProperty() : TABIPropertyWithValue<TSizedData<Size>>(TSizedData<Size>::Empty())
{
}

template <uint32 Size>
FABIStaticFixedBytesProperty<Size>::FABIStaticFixedBytesProperty(TSizedData<Size> initialValue): TABIPropertyWithValue<TSizedData<Size>>(initialValue)
{
}

template <uint32 Size>
FABIArg FABIStaticFixedBytesProperty<Size>::Serialize()
{
	auto BlockNum = (this->value.GetLength() + GBlockByteLength - 1) / GBlockByteLength;
	auto Data = new uint8[GBlockByteLength * BlockNum];
	
	for(auto i = 0u; i < GBlockByteLength * BlockNum; i++)
	{
		if(i < this->value.GetLength())
		{
			Data[i] = this->value.Ptr()[i];
		}
		else
		{
			Data[i] = 0;
		}
	}


	return FABIArg{STATIC, GBlockByteLength * BlockNum, Data};
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

	for(auto i = 0u; i < Size; i++)
	{
		Data[i] = ArgData[i];
	}
	auto mUniformData = TSizedData<Size>();
	*mUniformData.Arr.Get() = MakeArray(Data, Size);
	this->SetValue(mUniformData);
	Arg.Destroy();
}



