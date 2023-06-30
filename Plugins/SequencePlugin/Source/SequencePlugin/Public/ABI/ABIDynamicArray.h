#pragma once
#include "ABI/ABI.h"
#include "ABI/ABITypes.h"

template<typename T>
class FABIDynamicArrayProperty : public TABIPropertyWithValue<TArray<T>*>
{
public:
	FABIDynamicArrayProperty();
	FABIDynamicArrayProperty(TArray<T> *initialValue);
	virtual FABIArg Serialize() override;
	virtual FABIArg BlankArg() override;
	virtual void Deserialize(FABIArg Arg) override;
	virtual FABIProperty* Copy() override;
};

template <typename T>
FABIArg FABIDynamicArrayProperty<T>::Serialize()
{
	auto Length = (*this->value).Num();
	const auto Args = new FABIArg[Length];
    
	for(auto i = 0; i < Length; i++)
	{
		Args[i] = (*this->value)[i].Serialize();
	}

	return FABIArg{ARRAY, static_cast<uint32>(Length), Args};
}

template <typename T>
FABIArg FABIDynamicArrayProperty<T>::BlankArg()
{
	return FABIArg{ARRAY, 1, new FABIArg{T().BlankArg()}};
}

template <typename T>
void FABIDynamicArrayProperty<T>::Deserialize(FABIArg Arg)
{
	const FABIArg* Args = static_cast<const FABIArg*>(Arg.Data);

	this->value = new TArray<T>{};

    for(auto i = 0; i < Arg.Length; i++)
    {
    	this->value->Push(T());
    	FABIProperty* Prop = &(*this->value)[i];
    	Prop->Deserialize(Args[i]);
    }
}

template <typename T>
FABIProperty* FABIDynamicArrayProperty<T>::Copy()
{
	auto Array = new TArray<TABIPropertyWithValue<T>>();
	return new FABIDynamicArrayProperty<T>();
}

template <typename T>
FABIDynamicArrayProperty<T>::FABIDynamicArrayProperty(): TABIPropertyWithValue<TArray<T>*>(nullptr)
{
	static_assert(std::is_base_of_v<FABIProperty, T>, "type parameter of FABIDynamicArrayProperty must derive from FABIProperty");
}

template <typename T>
FABIDynamicArrayProperty<T>::FABIDynamicArrayProperty(TArray<T>* initialValue): TABIPropertyWithValue<TArray<T>*>(initialValue)
{
	static_assert(std::is_base_of_v<FABIProperty, T>, "type parameter of FABIDynamicArrayProperty must derive from FABIProperty");
}