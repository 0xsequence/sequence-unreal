#pragma once

#include "ABI/ABI.h"
#include "ABI/ABITypes.h"

template<typename A, typename B>
class FABIDynamicFixedTupleProperty : public TABIPropertyWithValue<TTuple<A, B>*>
{
	int Size = 2;
public:
	FABIDynamicFixedTupleProperty();
	FABIDynamicFixedTupleProperty(TTuple<A, B> *InitialValue);
	virtual FABIArg Serialize() override;
	virtual FABIArg BlankArg() override;
	virtual void Deserialize(FABIArg Arg) override;
};

template <typename A, typename B>
FABIDynamicFixedTupleProperty<A, B>::FABIDynamicFixedTupleProperty() : TABIPropertyWithValue<TTuple<A, B>*>(nullptr)
{
}

template <typename A, typename B>
FABIDynamicFixedTupleProperty<A, B>::FABIDynamicFixedTupleProperty(TTuple<A, B>* InitialValue) : TABIPropertyWithValue<TTuple<A, B>*>(InitialValue)
{
}

template <typename A, typename B>
FABIArg FABIDynamicFixedTupleProperty<A, B>::Serialize()
{
	const auto Args = new FABIArg[Size]{this->value->Key.Serialize(), this->value->Value.Serialize()};
	return FABIArg{FIXED, Size, Args};
}

template <typename A, typename B>
FABIArg FABIDynamicFixedTupleProperty<A, B>::BlankArg()
{
	auto Args = new FABIArg[Size]{A().BlankArg(), B().BlankArg()};
	return FABIArg{FIXED,  Size, Args};
}

template <typename A, typename B>
void FABIDynamicFixedTupleProperty<A, B>::Deserialize(FABIArg Arg)
{
	const FABIArg* Args = static_cast<const FABIArg*>(Arg.Data);
	this->value = new TTuple<A, B>{A(), B()};
	this->value->Key->Deserialize(Args[0]);
	this->value->Value->Deserialize(Args[1]);
	Arg.Destroy();
}
