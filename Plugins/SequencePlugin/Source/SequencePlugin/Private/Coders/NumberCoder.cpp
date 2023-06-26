#include "Coders/NumberCoder.h"

FString NumberCoder::Encode(FHash256 num) {
    auto retval = num.ToHex();
    return retval;

    /*
     try
            {
                BigInteger bgNumber;
                if (number.GetType() == typeof(int))
                {
                    bgNumber = new BigInteger((int)number);

                }
                else if (number.GetType() == typeof(uint))
                {
                    bgNumber = new BigInteger((uint)number);
                }
                else
                {
                    bgNumber = (BigInteger)number;
                }
                string encoded = EncodeSignedIntString(bgNumber, 64);
                return encoded;
            }
            catch (Exception ex)
            {
                Debug.LogError($"Error encoding number to string: {ex.Message}");
                return null;
            }
    */
}