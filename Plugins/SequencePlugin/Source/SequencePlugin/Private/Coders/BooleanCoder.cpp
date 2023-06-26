#include "Coders/BooleanCoder.h"

FString BooleanCoder::Encode(bool value) {
    if(value) {
    return FString::ChrN(63, '0') + '1';
    } else {
        return FString::ChrN(64, '0');
    }
}