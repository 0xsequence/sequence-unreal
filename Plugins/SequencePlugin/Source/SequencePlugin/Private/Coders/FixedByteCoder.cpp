#include "Coders/FixedByteCoder.h"
#include <string>    
#include <iostream>   
#include <sstream>  


FString FixedByteCoder::Encode(FString value) {
    FString encodedVal = "";
	FString suffix = FString::ChrN(64 - (value.Len()*2), '0');
    
    auto arr = value.GetCharArray();
    for(int i = 0; i < value.Len(); i++) {
    	char ch = arr[i];
    	std::stringstream sstream;
    	sstream << std::hex << (int)ch;
    	auto hexRep = sstream.str();
    	if(hexRep.length() < 2)
    	{
    		hexRep ='0' + hexRep;
    	}
    	encodedVal += FString(hexRep.data());
    }
	UE_LOG(LogTemp, Display, TEXT("Fixed byte: %s"), *(encodedVal+ suffix));
	return encodedVal + suffix;
	
    
}