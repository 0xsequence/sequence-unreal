#pragma once
#include "Coders/AddressCoder.h"
#include "Coders/BooleanCoder.h"
#include "Coders/FixedByteCoder.h"
#include "Coders/NumberCoder.h"

class MyProperty {
public:
    virtual FString encode() = 0;
};


template<typename T, typename C>
class PropertyWithValue : public MyProperty {
protected:
    T value;
    C myEncoder{};

public:
    PropertyWithValue(const T& initialValue) : value(initialValue) {};

    FString encode() override {
        return myEncoder.Encode(value);
    }
};

class AddressProperty : public PropertyWithValue<FString, AddressCoder>
{
public:
    AddressProperty(FString address):  PropertyWithValue(address) {}
};


class NumberProperty : public PropertyWithValue<FHash256, NumberCoder> {  // make it better for negative and positive values and maybe convert demical to hex option
public:
    NumberProperty(FHash256 initialValue): PropertyWithValue(initialValue) {}
}; 

class BooleanProperty : public PropertyWithValue<bool, BooleanCoder> {
    
    public:
    BooleanProperty(bool initialValue): PropertyWithValue(initialValue) {}
};

class FixedByteProperty : public PropertyWithValue<FString, FixedByteCoder> {
    
public:
    FixedByteProperty(FString initialValue): PropertyWithValue(initialValue) {}
};

/*
class TupleProperty: public Property {

};

class FixedArray: public Property {

};

class DynamicArray: public Property {

};

class Bytes: public Property {

};

class FixedBytes: public Property {

};

class StringProperty: public Property {

};*/

