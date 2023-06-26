#include "Coders/AddressCoder.h"

FString AddressCoder::Encode(FString address) {
                address.RemoveFromStart(TEXT("0x"), ESearchCase::IgnoreCase);

                const FString encodedString = (FString::ChrN(64-address.Len(), '0') + address).ToLower();
                UE_LOG(LogTemp, Display, TEXT("Encoded String %s"), *encodedString);
                return encodedString;
}