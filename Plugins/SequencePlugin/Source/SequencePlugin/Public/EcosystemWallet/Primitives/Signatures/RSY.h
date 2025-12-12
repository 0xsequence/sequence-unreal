#pragma once

#include "CoreMinimal.h"
#include "Types/BigInt.h"

class SEQUENCEPLUGIN_API FRSY
{
public:
    FBigInt R;
    FBigInt S;
    int32 YParity = 0;

    FRSY()
        : R(FBigInt("0")), S(FBigInt("0")), YParity(0) {}

    FRSY(const FBigInt& InR, const FBigInt& InS, int32 InYParity)
        : R(InR), S(InS), YParity(InYParity) {}

    TArray<uint8> Pack() const;
    
    TArray<uint8> Pack65() const;

    // RSY inside string ("r:s:v")
    static TSharedPtr<FRSY> FromString(const FString& Input);

    // 64 bytes long encoded rsy (y inside s)
    static TSharedPtr<FRSY> Unpack(const TArray<uint8>& RSY);

    // 65 bytes long encoded rsy (r,s,v)
    static TSharedPtr<FRSY> UnpackFrom65(const TArray<uint8>& Sig65);

    static int32 VToYParity(int64 V)
    {
        if (V == 0 || V == 1)
            return V;
        
        if (V == 27) return 0;
        if (V == 28) return 1;
        return V % 2;
    }

    FString ToString() const
    {
        return FString::Printf(TEXT("R: %s, S: %s, Y: %d"), *R.Value, *S.Value, YParity);
    }

private:

    // --- Get parity of bigint represented as hex ---
    static int64 BigIntMod2(const FString& Hex)
    {
        FString Clean = Hex.Replace(TEXT("0x"), TEXT(""));
        if (Clean.IsEmpty())
            return 0;

        TCHAR Last = Clean[Clean.Len() - 1];
        int32 Value = FParse::HexDigit(Last);
        return Value % 2;
    }
};
