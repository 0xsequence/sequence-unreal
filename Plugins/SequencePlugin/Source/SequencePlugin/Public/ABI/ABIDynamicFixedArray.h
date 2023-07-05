#pragma once

#include "ABI/ABI.h"
#include "ABI/ABITypes.h"

template<typename T, uint32 Size>
class FABIDynamicFixedArrayProperty : public TABIPropertyWithValue<TArray<T>*>
{
public:
	FABIDynamicFixedArrayProperty();
	FABIDynamicFixedArrayProperty(TArray<T>* initialValue);
	virtual FABIArg Serialize() override;
	virtual FABIArg BlankArg() override;
	virtual void Deserialize(FABIArg Arg) override;
};

template <typename T, uint32 Size>
FABIDynamicFixedArrayProperty<T, Size>::FABIDynamicFixedArrayProperty() : TABIPropertyWithValue<TArray<T>*>(nullptr)
{
	static_assert(std::is_base_of_v<FABIProperty, T>, "type parameter of FABIFixedArrayProperty must derive from FABIProperty");
}

template <typename T, uint32 Size>
FABIDynamicFixedArrayProperty<T, Size>::FABIDynamicFixedArrayProperty(TArray<T>* initialValue) : TABIPropertyWithValue<TArray<T>*>(initialValue)
{
	static_assert(std::is_base_of_v<FABIProperty, T>, "type parameter of FABIFixedArrayProperty must derive from FABIProperty");
}

template <typename T, uint32 Size>
FABIArg FABIDynamicFixedArrayProperty<T, Size>::Serialize()
{
	auto Length = (*this->value).Num();
	const auto Args = new FABIArg[Length];
    
	for(auto i = 0; i < Size; i++)
	{
		Args[i] = (*this->value)[i].Serialize();
	}

	return FABIArg{FIXED, Size, Args};
}

template <typename T, uint32 Size>
FABIArg FABIDynamicFixedArrayProperty<T, Size>::BlankArg()
{
	auto Args = new FABIArg[Size];
	for(auto i = 0; i < Size; i++)
	{
		Args[i] = T().BlankArg();
	}
	
	return FABIArg{FIXED,  Size, Args};
}

template <typename T, uint32 Size>
void FABIDynamicFixedArrayProperty<T, Size>::Deserialize(FABIArg Arg)
{
	const FABIArg* Args = static_cast<const FABIArg*>(Arg.Data);

	this->value = new TArray<T>{};

	for(uint32 i = 0; i < Arg.Length; i++)
	{
		this->value->Push(T());
		FABIProperty* Prop = &(*this->value)[i];
		Prop->Deserialize(Args[i]);
	}

	Arg.Destroy();
}
