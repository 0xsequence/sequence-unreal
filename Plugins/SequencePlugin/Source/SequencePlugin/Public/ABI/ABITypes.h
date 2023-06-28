#pragma once
#include "Coders/AddressCoder.h"
#include "Coders/BooleanCoder.h"
#include "Coders/FixedByteCoder.h"
#include "Coders/NumberCoder.h"
#include <type_traits>

struct FABIArg;

class FABIProperty {
public:
    virtual ~FABIProperty() = default;
    virtual FABIArg Serialize() = 0;
    virtual void Deserialize(FABIArg Arg) = 0;
    virtual FABIProperty* Copy() = 0;
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
    FNonUniformData BinaryData;
public:
    FABIStringProperty();
    FABIStringProperty(FString InitialValue);
    ~FABIStringProperty();
    virtual FABIArg Serialize() override;
    virtual void Deserialize(FABIArg Arg) override;
    virtual FABIProperty* Copy() override;
};

class FABIIntProperty : public TABIPropertyWithValue<int32> {
public:
    FABIIntProperty();
    FABIIntProperty(const int32 InitialValue);
    virtual FABIArg Serialize() override;
    virtual void Deserialize(FABIArg Arg) override;
    virtual FABIProperty* Copy() override;
};

class FABIUIntProperty : public TABIPropertyWithValue<uint32> {
public:
    FABIUIntProperty();
    FABIUIntProperty(uint32 InitialValue);
    virtual FABIArg Serialize() override;
    virtual void Deserialize(FABIArg Arg) override;
    virtual FABIProperty* Copy() override;
}; 

class FABIBooleanProperty : public TABIPropertyWithValue<bool> {
public:
    FABIBooleanProperty();
    FABIBooleanProperty(bool bInitialValue);
    virtual FABIArg Serialize() override;
    virtual void Deserialize(FABIArg Arg) override;
    virtual FABIProperty* Copy() override;
};

class FABIBytesProperty : public TABIPropertyWithValue<FNonUniformData> {
public:
    FABIBytesProperty();
    FABIBytesProperty(FNonUniformData InitialValue);
    virtual FABIArg Serialize() override;
    virtual void Deserialize(FABIArg Arg) override;
    virtual FABIProperty* Copy() override;
};

class FABIAddressProperty : public TABIPropertyWithValue<FAddress>
{
public:
    FABIAddressProperty();
    FABIAddressProperty(FAddress Address);
    virtual FABIArg Serialize() override;
    virtual void Deserialize(FABIArg Arg) override;
    virtual FABIProperty* Copy() override;
};


template<typename T>
class FABIArrayProperty : public TABIPropertyWithValue<TArray<T>*>
{
public:
    FABIArrayProperty();
    FABIArrayProperty(TArray<T> *initialValue);
    virtual FABIArg Serialize() override;
    virtual void Deserialize(FABIArg arg) override;
    virtual FABIProperty* Copy() override;
};

