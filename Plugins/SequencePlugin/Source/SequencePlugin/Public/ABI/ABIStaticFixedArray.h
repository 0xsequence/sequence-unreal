#pragma once
#include "Types/BinaryData.h"
#include "ABI/ABI.h"
#include "ABI/ABITypes.h"

template<typename T, uint32 Size>
class FABIStaticFixedArrayProperty : public TABIPropertyWithValue<TArray<T>*>
{
	ByteLength ElementSize();
public:
	FABIStaticFixedArrayProperty();
	FABIStaticFixedArrayProperty(TArray<T>* initialValue);
	virtual FABIArg Serialize() override;
	virtual FABIArg BlankArg() override;
	virtual void Deserialize(FABIArg Arg) override;
};

template <typename T, uint32 Size>
ByteLength FABIStaticFixedArrayProperty<T, Size>::ElementSize()
{
	T Prop = T();
	return Prop.BlankArg().Length;
}

template <typename T, uint32 Size>
FABIStaticFixedArrayProperty<T, Size>::FABIStaticFixedArrayProperty() : TABIPropertyWithValue<TArray<T>*>(nullptr)
{
	static_assert(std::is_base_of_v<FABIProperty, T>, "type parameter of FABIStaticFixedArrayProperty must derive from FABIProperty");
}

template <typename T, uint32 Size>
FABIStaticFixedArrayProperty<T, Size>::FABIStaticFixedArrayProperty(TArray<T>* initialValue) : TABIPropertyWithValue<TArray<T>*>(initialValue)
{
	static_assert(std::is_base_of_v<FABIProperty, T>, "type parameter of FABIStaticFixedArrayProperty must derive from FABIProperty");
}

template <typename T, uint32 Size>
FABIArg FABIStaticFixedArrayProperty<T, Size>::Serialize()
{
	uint8 ElementLength = ElementSize();
	uint8* Data = new uint8[ElementLength * Size];

	for(auto i = 0; i < Size; i++)
	{
		FABIArg Arg = (*this->value)[i].Serialize();
		uint8* ArgData = static_cast<uint8*>(Arg.Data);

		for(auto j = 0; j < ElementLength; j++)
		{
			Data[i * ElementLength + j] = ArgData[j];
		}
		
		Arg.Destroy();
	}

	return FABIArg{
		STATIC, ElementLength * Size, Data
	};
}

template <typename T, uint32 Size>
FABIArg FABIStaticFixedArrayProperty<T, Size>::BlankArg()
{
	return FABIArg{
		STATIC, ElementSize() * Size, nullptr
	};
}

template <typename T, uint32 Size>
void FABIStaticFixedArrayProperty<T, Size>::Deserialize(FABIArg Arg)
{
	uint8* Data = static_cast<uint8*>(Arg.Data);
	this->value = new TArray<T>{};
	auto ElementLength = ElementSize();

	for(int i = 0; i < Size; i++)
	{
		T Element = T();
		Element.Deserialize(FABIArg{
			STATIC, ElementLength, &Data[i * ElementLength]
		});

		(*this->value).Push(Element);
	}

	Arg.Destroy();
}



