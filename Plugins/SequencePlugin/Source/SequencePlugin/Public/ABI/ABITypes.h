#pragma once

#include "Types/BinaryData.h"

struct FABIArg;

class FABIProperty {
public:
    virtual ~FABIProperty() = default;
    virtual FABIArg Serialize() = 0;
    virtual FABIArg BlankArg() = 0;
    virtual void Deserialize(FABIArg Arg) = 0;
};

template<typename T>
class TABIPropertyWithValue : public FABIProperty {
protected:
    T value;

public:
    TABIPropertyWithValue();
    TABIPropertyWithValue(const T& initialValue);
    virtual T GetValue();
    virtual void SetValue(T t);
};

template <typename T>
TABIPropertyWithValue<T>::TABIPropertyWithValue(): value()
{}

template <typename T>
TABIPropertyWithValue<T>::TABIPropertyWithValue(const T& initialValue): value(initialValue)
{}

template <typename T>
T TABIPropertyWithValue<T>::GetValue()
{
    return value;
}

template <typename T>
void TABIPropertyWithValue<T>::SetValue(T t)
{
    value = t;
}

class FABIStringProperty : public TABIPropertyWithValue<FString> {
public:
    FABIStringProperty();
    FABIStringProperty(FString InitialValue);
    virtual FABIArg Serialize() override;
    virtual FABIArg BlankArg() override;
    virtual void Deserialize(FABIArg Arg) override;
};

class FABIInt32Property : public TABIPropertyWithValue<int32> {
public:
    FABIInt32Property();
    FABIInt32Property(const int32 InitialValue);
    virtual FABIArg Serialize() override;
    virtual FABIArg BlankArg() override;
    virtual void Deserialize(FABIArg Arg) override;
};

class FABIUInt32Property : public TABIPropertyWithValue<uint32> {
public:
    FABIUInt32Property();
    FABIUInt32Property(uint32 InitialValue);
    virtual FABIArg Serialize() override;
    virtual FABIArg BlankArg() override;
    virtual void Deserialize(FABIArg Arg) override;
}; 

class FABIBooleanProperty : public TABIPropertyWithValue<bool> {
public:
    FABIBooleanProperty();
    FABIBooleanProperty(bool bInitialValue);
    virtual FABIArg Serialize() override;
    virtual FABIArg BlankArg() override;
    virtual void Deserialize(FABIArg Arg) override;
};

class FABIBytesProperty : public TABIPropertyWithValue<FNonUniformData> {
public:
    FABIBytesProperty();
    FABIBytesProperty(FNonUniformData InitialValue);
    virtual FABIArg Serialize() override;
    virtual FABIArg BlankArg() override;
    virtual void Deserialize(FABIArg Arg) override;
};

class FABIAddressProperty : public TABIPropertyWithValue<FAddress>
{
public:
    FABIAddressProperty();
    FABIAddressProperty(FAddress Address);
    virtual FABIArg Serialize() override;
    virtual FABIArg BlankArg() override;
    virtual void Deserialize(FABIArg Arg) override;
};
