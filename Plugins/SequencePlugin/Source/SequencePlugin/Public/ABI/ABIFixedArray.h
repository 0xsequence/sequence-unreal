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
};

template <typename T, uint32 Size>
FABIFixedArrayProperty<T, Size>::FABIFixedArrayProperty()
{
	static_assert(std::is_base_of_v<FABIProperty, T>, "type parameter of FABIFixedArrayProperty must derive from FABIProperty");
}

template <typename T, uint32 Size>
FABIFixedArrayProperty<T, Size>::FABIFixedArrayProperty(T* initialValue)
{
	static_assert(std::is_base_of_v<FABIProperty, T>, "type parameter of FABIFixedArrayProperty must derive from FABIProperty");
}

template <typename T, uint32 Size>
FABIArg FABIFixedArrayProperty<T, Size>::Serialize()
{
	const auto Args = new FABIArg[Size];
    
	for(auto i = 0; i < Size; i++)
	{
		Args[i] = this->value[i].Serialize();
	}
	
	return FABIArg{ARRAY, Size, Args};
}

template <typename T, uint32 Size>
FABIArg FABIFixedArrayProperty<T, Size>::BlankArg()
{
	return FABIArg{STATIC, T().BlankArg().Length * Size, nullptr};
}

template <typename T, uint32 Size>
void FABIFixedArrayProperty<T, Size>::Deserialize(FABIArg Arg)
{
	const FABIArg* Args = static_cast<const FABIArg*>(Arg.Data);

	this->value = new T[Size];

	for(uint32 i = 0; i < Arg.Length; i++)
	{
		this->value[i] = T();
		FABIProperty* Prop = &(this->value[i]);
		Prop->Deserialize(Args[i]);
	}
}