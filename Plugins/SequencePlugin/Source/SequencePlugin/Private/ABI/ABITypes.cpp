#include "ABI/ABITypes.h"
#include "ABI/ABI.h"
#include "IOS/Accessibility/IOSAccessibilityElement.h"

FABIStringProperty::FABIStringProperty(): TABIPropertyWithValue(), BinaryData(FNonUniformData::Empty())
{}

FABIStringProperty::FABIStringProperty(FString InitialValue): TABIPropertyWithValue(InitialValue), BinaryData(StringToUTF8(InitialValue))
{
}

FABIStringProperty::~FABIStringProperty()
{
	delete [] BinaryData.Arr;
}

FABIArg FABIStringProperty::Serialize()
{
	return FABIArg {
		STRING,
		BinaryData.Length,
		BinaryData.Arr
	};
}

void FABIStringProperty::Deserialize(FABIArg Arg)
{
	ByteLength Length = 0;
	const auto Data = static_cast<uint8*>(Arg.Data);
	
	while(Data[Length] != 0x00)
	{
		Length += 1;
	}

	const auto StringData = FNonUniformData
	{
		Data, Length
	};

	const auto String = UTF8ToString(StringData);
	SetValue(String);
	Arg.Destroy();
}

FABIProperty* FABIStringProperty::Copy()
{
	return new FABIStringProperty(value);
}

FABIIntProperty::FABIIntProperty(): TABIPropertyWithValue(0)
{}

FABIIntProperty::FABIIntProperty(const int32 InitialValue): TABIPropertyWithValue(InitialValue)
{}

FABIArg FABIIntProperty::Serialize()
{
	const auto ArgData = NewEmptyBlock();
	const auto CopyData = static_cast<uint8*>(static_cast<void*>(&value));

	ArgData.Arr[GBlockByteLength - 4] = CopyData[0];
	ArgData.Arr[GBlockByteLength - 3] = CopyData[1];
	ArgData.Arr[GBlockByteLength - 2] = CopyData[2];
	ArgData.Arr[GBlockByteLength - 1] = CopyData[3];
	
	return FABIArg{
		STATIC, GBlockByteLength, ArgData.Arr
	};
}

void FABIIntProperty::Deserialize(FABIArg Arg)
{
	const auto Data = static_cast<uint8*>(Arg.Data);
	const auto DataPointer = static_cast<void*>(&Data[GBlockByteLength - 4]);
	SetValue(*static_cast<int32*>(DataPointer)); // We reinterpret the data at the pointer to be an int32
	Arg.Destroy();
}

FABIProperty* FABIIntProperty::Copy()
{
	return new FABIIntProperty(value);
}

FABIUIntProperty::FABIUIntProperty(): TABIPropertyWithValue<unsigned>(0)
{}

FABIUIntProperty::FABIUIntProperty(uint32 InitialValue): TABIPropertyWithValue(InitialValue)
{}

FABIArg FABIUIntProperty::Serialize()
{
	const auto ArgData = NewEmptyBlock();
	const auto CopyData = static_cast<uint8*>(static_cast<void*>(&value));

	ArgData.Arr[GBlockByteLength - 4] = CopyData[0];
	ArgData.Arr[GBlockByteLength - 3] = CopyData[1];
	ArgData.Arr[GBlockByteLength - 2] = CopyData[2];
	ArgData.Arr[GBlockByteLength - 1] = CopyData[3];
	
	return FABIArg{
		STATIC, GBlockByteLength, ArgData.Arr
	};
}

void FABIUIntProperty::Deserialize(FABIArg Arg)
{
	const auto Data = static_cast<uint8*>(Arg.Data);
	const auto DataPointer = static_cast<void*>(&Data[GBlockByteLength - 4]);
	SetValue(*static_cast<uint32*>(DataPointer)); // We reinterpret the data at the pointer to be a uint32
	Arg.Destroy();
}

FABIProperty* FABIUIntProperty::Copy()
{
	return new FABIUIntProperty(value);
}

FABIBooleanProperty::FABIBooleanProperty(): TABIPropertyWithValue<bool>(false)
{}

FABIBooleanProperty::FABIBooleanProperty(bool bInitialValue): TABIPropertyWithValue(bInitialValue)
{}

FABIArg FABIBooleanProperty::Serialize()
{
	const auto ArgData = NewEmptyBlock();

	if(value)
	{
		ArgData.Arr[GBlockByteLength - 1] = 0x01;
	}

	return FABIArg{
		STATIC, GBlockByteLength, ArgData.Arr
	};
}

void FABIBooleanProperty::Deserialize(FABIArg Arg)
{
	const auto Data = static_cast<uint8*>(Arg.Data);
	SetValue(Data[GBlockByteLength - 1] == 0x01);
	Arg.Destroy();
}

FABIProperty* FABIBooleanProperty::Copy()
{
	return new FABIBooleanProperty(value);
}

FABIBytesProperty::FABIBytesProperty(): TABIPropertyWithValue<FNonUniformData>(FNonUniformData::Empty())
{
}

FABIBytesProperty::FABIBytesProperty(FNonUniformData InitialValue): TABIPropertyWithValue(InitialValue)
{}

FABIArg FABIBytesProperty::Serialize()
{
	const auto Data = new uint8[value.Length];

	for(auto i = 0; i < value.Length; i++)
	{
		Data[i] = value.Arr[i];
	}

	return FABIArg{
		BYTES, value.Length, Data
	};
}

void FABIBytesProperty::Deserialize(FABIArg Arg)
{
	const auto CopyData = static_cast<uint8*>(Arg.Data);
	const auto Data = new uint8[Arg.Length];

	for(auto i = 0; i < Arg.Length; i++)
	{
		Data[i] = CopyData[i];
	}

	SetValue(FNonUniformData{
		Data, Arg.Length
	});
	Arg.Destroy();
}

FABIProperty* FABIBytesProperty::Copy()
{
	return new FABIBytesProperty(value.Copy());
}

FABIAddressProperty::FABIAddressProperty(): TABIPropertyWithValue(FAddress{})
{}

FABIAddressProperty::FABIAddressProperty(FAddress Address):  TABIPropertyWithValue(Address)
{}

FABIArg FABIAddressProperty::Serialize()
{
	FNonUniformData ArgData = NewEmptyBlock();

	for(auto i = 0; i < FAddress::Size; i++)
	{
		ArgData.Arr[i] = value.Arr[i];
	}

	return FABIArg{
		STATIC, FAddress::Size, ArgData.Arr
	};
}

void FABIAddressProperty::Deserialize(FABIArg Arg)
{
	const auto CopyData = static_cast<uint8*>(Arg.Data);
	const auto Address = FAddress::New();

	for(auto i = 0; i < FAddress::Size; i++)
	{
		Address.Arr[i] = CopyData[i];
	}

	SetValue(Address);
	Arg.Destroy();
}

FABIProperty* FABIAddressProperty::Copy()
{
	return new FABIAddressProperty(FAddress::From(value.ToHex()));
}

template <typename T>
FABIArrayProperty<T>::FABIArrayProperty(): TABIPropertyWithValue<TArray<T>*>(T())
{
	static_assert(std::is_base_of_v<FABIProperty, T>, "type parameter of FABIArrayProperty must derive from FABIProperty");
}

template <typename T>
FABIArrayProperty<T>::FABIArrayProperty(TArray<T>* initialValue): TABIPropertyWithValue<TArray<T>*>(initialValue)
{
	static_assert(std::is_base_of_v<FABIProperty, T>, "type parameter of FABIArrayProperty must derive from FABIProperty");
}

template <typename T>
FABIArg FABIArrayProperty<T>::Serialize()
{
	auto Length = this->value.Num();
	const auto Args = new FABIArg[2];
    
	for(auto i = 0; i < Length; i++)
	{
		Args[i] = (*this->value)[i].Serialize();
	}

	return FABIArg{ARRAY, Length, Args};
}

template <typename T>
void FABIArrayProperty<T>::Deserialize(FABIArg arg)
{
	const FABIArg* Args = static_cast<const FABIArg*>(arg.Data);

    
}

template <typename T>
FABIProperty* FABIArrayProperty<T>::Copy()
{
	auto Array = new TArray<TABIPropertyWithValue<T>>();
	return new FABIArrayProperty<T>();
}

