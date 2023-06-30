#pragma once

#include "ABI/ABI.h"
#include "ABI/ABITypes.h"

template<typename T, uint32 Size>
class FABIFixedArrayProperty : public TABIPropertyWithValue<T[]>
{
public:
	FABIFixedArrayProperty();
	FABIFixedArrayProperty(T initialValue[]);
	virtual FABIArg Serialize() override;
	virtual FABIArg BlankArg() override;
	virtual void Deserialize(FABIArg Arg) override;
	virtual FABIProperty* Copy() override;
};

template <typename T, uint32 Size>
FABIFixedArrayProperty<T, Size>::FABIFixedArrayProperty()
{
	static_assert(std::is_base_of_v<FABIProperty, T>, "type parameter of FABIFixedArrayProperty must derive from FABIProperty");
}

template <typename T, uint32 Size>
FABIFixedArrayProperty<T, Size>::FABIFixedArrayProperty(T initialValue[])
{
	static_assert(std::is_base_of_v<FABIProperty, T>, "type parameter of FABIFixedArrayProperty must derive from FABIProperty");
}

template <typename T, uint32 Size>
FABIArg FABIFixedArrayProperty<T, Size>::Serialize()
{
	
}
