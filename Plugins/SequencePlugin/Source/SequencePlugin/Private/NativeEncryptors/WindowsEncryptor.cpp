//Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "NativeEncryptors/WindowsEncryptor.h"
#include "Misc/Base64.h"
#include "Internationalization/Regex.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#include "dpapi.h"
#endif

FString UWindowsEncryptor::Encrypt(const FString& StringIn)
{
	FString Result = "";
	UE_LOG(LogTemp,Display,TEXT("Preparing to encrypt on windows: %s"),*StringIn);
#if PLATFORM_WINDOWS
	DATA_BLOB DataIn;
	DATA_BLOB DataOut;
	const FString ProcString = FBase64::Encode(StringIn);

	//UE_LOG(LogTemp,Display,TEXT("Decrypted PreResult: %s"),*ProcString);
	//UE_LOG(LogTemp,Display,TEXT("PreResult Length: %d"),ProcString.Len());
	
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
	delete[] CharsIn;
#endif
	//UE_LOG(LogTemp,Display,TEXT("Encrypted Result: %s"),*Result);
	return Result;
}

FString UWindowsEncryptor::Decrypt(const FString& StringIn)
{
	FString Result = "";
	UE_LOG(LogTemp,Display,TEXT("Preparing to decrypt on windows: %s"),*StringIn);
#if PLATFORM_WINDOWS
	DATA_BLOB DataIn;
	DATA_BLOB DataOut;

	const int32 InSize = StringIn.Len() / 2;
	uint8 * CharsIn = new uint8[InSize];

	const FRegexPattern HexPattern(TEXT("^[a-fA-F0-9]+$"));
	FRegexMatcher HexChecker(HexPattern,StringIn);
	
	if (HexChecker.FindNext())
	{
		HexToBytes(StringIn,CharsIn);
	}
	else
	{
		delete[] CharsIn;
		UE_LOG(LogTemp,Error,TEXT("Provided String is InValid and cannot be decoded!"));
		return "";
	}
	
	BYTE *pbDataInput = CharsIn;
	const DWORD cbDataInput = InSize;	
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
		const FString PreResult = BytesToString(DataOut.pbData,DataOut.cbData);

		//UE_LOG(LogTemp,Display,TEXT("Decrypted PreResult: %s"),*PreResult);
		//UE_LOG(LogTemp,Display,TEXT("PreResult Length: %d"),PreResult.Len());
		
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
	delete[] CharsIn;
#endif
	//UE_LOG(LogTemp,Display,TEXT("Decrypted Result: %s"),*Result);
	return Result;
}
