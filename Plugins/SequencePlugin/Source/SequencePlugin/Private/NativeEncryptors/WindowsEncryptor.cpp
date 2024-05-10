//Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "NativeEncryptors/WindowsEncryptor.h"
#include "Types/BinaryData.h"
#include "Util/HexUtility.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#include "dpapi.h"
#endif

FString UWindowsEncryptor::Encrypt(const FString& StringIn)
{
	FString Result = "";
	UE_LOG(LogTemp,Display,TEXT("Preparing to encrypt on windows: %s"),*StringIn);
	UE_LOG(LogTemp,Display,TEXT("InLength: %d"),StringIn.Len());
#if PLATFORM_WINDOWS
	DATA_BLOB DataIn;
	DATA_BLOB DataOut;
	const FString ProcString = FBase64::Encode(StringIn);

	UE_LOG(LogTemp,Display,TEXT("Decrypted PreResult: %s"),*ProcString);
	UE_LOG(LogTemp,Display,TEXT("PreResult Length: %d"),ProcString.Len());
	
	const int32 InSize = ProcString.Len();
	uint8 * CharsIn =  new uint8[InSize];
	StringToBytes(ProcString,CharsIn,InSize);
	BYTE *pbDataInput = CharsIn;
	const DWORD cbDataInput = InSize;
	UE_LOG(LogTemp,Display,TEXT("InByteCount: %d"),cbDataInput);
	
	DataIn.pbData = pbDataInput;
	DataIn.cbData = cbDataInput;
	if (CryptProtectData(
		&DataIn,
		NULL,
		NULL,
		NULL,
		NULL,
		0,
		&DataOut))
	{
		BytesToHex(DataOut.pbData,DataOut.cbData,Result);
		LocalFree(DataOut.pbData);
	}
	else
	{
		UE_LOG(LogTemp,Display,TEXT("Encryption Failed on windows"));
	}
	//delete[] CharsIn;
#endif
	UE_LOG(LogTemp,Display,TEXT("Encrypted Result: %s"),*Result);
	UE_LOG(LogTemp,Display,TEXT("OutLength: %d"),Result.Len());
	return Result;
}

FString UWindowsEncryptor::Decrypt(const FString& StringIn)
{
	FString Result = "";
	UE_LOG(LogTemp,Display,TEXT("Preparing to decrypt on windows: %s"),*StringIn);
	UE_LOG(LogTemp,Display,TEXT("InLength: %d"),StringIn.GetCharArray().Num());
#if PLATFORM_WINDOWS
	DATA_BLOB DataIn;
	DATA_BLOB DataOut;
	
	//Copy in our FString data into a Char* array
	/*const int32 InSize = StringIn.GetCharArray().Num();
	char * CharsIn =  new char[InSize];
	for (int i = 0; i < InSize; i++)
		CharsIn[i] = (char)StringIn.GetCharArray()[i];*/

	//char * InBytes = TCHAR_TO_ANSI(*StringIn);
	/*const char * InBytes = StringCast<ANSICHAR>(*StringIn).Get();
	const BYTE *pbDataInput = reinterpret_cast<const BYTE*>(InBytes);
	
	//const DWORD cbDataInput = StringIn.Len() * sizeof(TCHAR);
	const DWORD cbDataInput = strlen((char *)pbDataInput);
	UE_LOG(LogTemp,Display,TEXT("InByteCount: %d"),cbDataInput);*/

	/*const FUnsizedData InBytes = StringToUTF8(StringIn);
	BYTE *pbDataInput = InBytes.Arr->GetData();
	const DWORD cbDataInput = strlen((char *)pbDataInput)+1;
	UE_LOG(LogTemp,Display,TEXT("InByteCount: %d"),cbDataInput);*/

	/*const char * InBytes = StringCast<ANSICHAR>(*StringIn).Get();
	const BYTE *pbDataInput = reinterpret_cast<const BYTE*>(InBytes);
	const DWORD cbDataInput = strlen((char *)pbDataInput);
	UE_LOG(LogTemp,Display,TEXT("InByteCount: %d"),cbDataInput);*/

	//Copy in our FString data into a Char* array

	const int32 InSize = StringIn.Len() / 2;
	uint8 * CharsIn = new uint8[InSize];
	HexToBytes(StringIn,CharsIn);
	
	/*const int32 InSize = StringIn.GetCharArray().Num();
	char * CharsIn =  new char[InSize];
	for (int i = 0; i < InSize; i++)
		CharsIn[i] = static_cast<char>(StringIn.GetCharArray()[i]);*/
	
	BYTE *pbDataInput = CharsIn;
	const DWORD cbDataInput = InSize;
	UE_LOG(LogTemp,Display,TEXT("InByteCount: %d"),cbDataInput);
	
	DataIn.pbData = pbDataInput;
	DataIn.cbData = cbDataInput;
	LPWSTR pDescrOut = NULL;
	if (CryptUnprotectData(
		&DataIn,
		&pDescrOut,
		NULL,
		NULL,
		NULL,
		0,
		&DataOut))
	{
		//const char * OutBytes = reinterpret_cast<char *>(DataOut.pbData);
		//const int32 OutSize = DataOut.cbData;

		const FString PreResult = BytesToString(DataOut.pbData,DataOut.cbData);

		UE_LOG(LogTemp,Display,TEXT("Decrypted PreResult: %s"),*PreResult);
		UE_LOG(LogTemp,Display,TEXT("PreResult Length: %d"),PreResult.Len());
		
		if (FBase64::Decode(PreResult,Result))
		{
			UE_LOG(LogTemp,Display,TEXT("Successful B64 Decode"));
		}
		else
		{
			UE_LOG(LogTemp,Error,TEXT("UnSuccessful B64 Decode"));
		}
		
		LocalFree(DataOut.pbData);
		LocalFree(pDescrOut);
	}
	else
	{
		const int32 ErrorCode = GetLastError();
		UE_LOG(LogTemp,Display,TEXT("Decryption Failed on windows, Error code: %d"),ErrorCode);
	}
#endif
	UE_LOG(LogTemp,Display,TEXT("Decrypted Result: %s"),*Result);
	UE_LOG(LogTemp,Display,TEXT("OutLength: %d"),Result.Len());
	return Result;
}
