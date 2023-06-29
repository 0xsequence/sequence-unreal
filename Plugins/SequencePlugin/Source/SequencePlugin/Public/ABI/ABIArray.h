#pragma once
#include "ABI/ABI.h"
#include "ABI/ABITypes.h"

template<typename T>
class FABIArrayProperty : public TABIPropertyWithValue<TArray<T>*>
{
public:
	FABIArrayProperty();
	FABIArrayProperty(TArray<T> *initialValue);
	virtual FABIArg Serialize() override;
	virtual void Deserialize(FABIArg Arg) override;
	virtual FABIProperty* Copy() override;
};

template <typename T>
FABIArg FABIArrayProperty<T>::Serialize()
{
	auto Length = (*this->value).Num();
	const auto Args = new FABIArg[2];
    
	for(auto i = 0; i < Length; i++)
	{
		Args[i] = (*this->value)[i].Serialize();
	}

	return FABIArg{ARRAY, Length, Args};
}

template <typename T>
void FABIArrayProperty<T>::Deserialize(FABIArg Arg)
{
	const FABIArg* Args = static_cast<const FABIArg*>(Arg.Data);

    
}

template <typename T>
FABIProperty* FABIArrayProperty<T>::Copy()
{
	auto Array = new TArray<TABIPropertyWithValue<T>>();
	return new FABIArrayProperty<T>();
}

template <typename T>
FABIArrayProperty<T>::FABIArrayProperty(): TABIPropertyWithValue<TArray<T>*>(nullptr)
{
	static_assert(std::is_base_of_v<FABIProperty, T>, "type parameter of FABIArrayProperty must derive from FABIProperty");
}

template <typename T>
FABIArrayProperty<T>::FABIArrayProperty(TArray<T>* initialValue): TABIPropertyWithValue<TArray<T>*>(initialValue)
{
	static_assert(std::is_base_of_v<FABIProperty, T>, "type parameter of FABIArrayProperty must derive from FABIProperty");
}