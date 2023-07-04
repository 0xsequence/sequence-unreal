#pragma once

#include "ABI/ABI.h"
#include "ABI/ABITypes.h"

template<typename T, uint32 Size>
class FABIFixedArrayProperty : public TABIPropertyWithValue<T*>
{
public:
	FABIFixedArrayProperty();
	FABIFixedArrayProperty(T initialValue[]);
	virtual FABIArg Serialize() override;
	virtual FABIArg BlankArg() override;
	virtual void Deserialize(FABIArg Arg) override;
	static uint32 GetBlockSize();
	static uint32 GetUnitBlockSize();
};

template <typename T, uint32 Size>
FABIFixedArrayProperty<T, Size>::FABIFixedArrayProperty()
{
	static_assert(std::is_base_of_v<FABIProperty, T>, "type parameter of FABIFixedArrayProperty must derive from FABIProperty");
}

template <typename T, uint32 Size>
FABIFixedArrayProperty<T, Size>::FABIFixedArrayProperty(T* initialValue) : TABIPropertyWithValue<T*>(initialValue)
{
	static_assert(std::is_base_of_v<FABIProperty, T>, "type parameter of FABIFixedArrayProperty must derive from FABIProperty");
}

template <typename T, uint32 Size>
FABIArg FABIFixedArrayProperty<T, Size>::Serialize()
{
	auto ByteLength = GetBlockSize() * GBlockByteLength;
	auto Data = new uint8[ByteLength];

	for(auto i = 0; i < ByteLength; i++)
	{
		Data[i] = 0x00;
	}
    
	for(auto i = 0; i < Size; i++)
	{
		FABIArg Arg = this->value[i].Serialize();
		const int Offset = i * GetUnitBlockSize() * GBlockByteLength;
		
		for(auto j = 0; j < Arg.Length; j++)
		{
			const auto ArgData = static_cast<uint8*>(Arg.Data);
			Data[Offset + j] = ArgData[j];
		}
		
		Arg.Destroy();
	}

	
	return FABIArg{STATIC, GetBlockSize() * GBlockByteLength, Data};
}

template <typename T, uint32 Size>
FABIArg FABIFixedArrayProperty<T, Size>::BlankArg()
{
	return FABIArg{STATIC,  GetBlockSize() * GBlockByteLength, nullptr};
}

template <typename T, uint32 Size>
void FABIFixedArrayProperty<T, Size>::Deserialize(FABIArg Arg)
{
	auto ArgData = static_cast<uint8*>(Arg.Data);

	this->value = new T[Size];

	for(uint32 i = 0; i < Arg.Length; i++)
	{
		this->value[i] = T();
		FABIProperty* Prop = &(this->value[i]);
		Prop->Deserialize(FABIArg{
			STATIC, T().BlankArg().Length, &ArgData[i * GetUnitBlockSize()]
		});
	}

	Arg.Destroy();
}

template <typename T, uint32 Size>
uint32 FABIFixedArrayProperty<T, Size>::GetBlockSize()
{
	return GetUnitBlockSize() * Size;
}

template <typename T, uint32 Size>
uint32 FABIFixedArrayProperty<T, Size>::GetUnitBlockSize()
{
	uint32 UnitSize = T().BlankArg().Length;
	return (UnitSize + GBlockByteLength - 1) / GBlockByteLength;
}
