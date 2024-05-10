//Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "NativeEncryptors/WindowsEncryptor.h"

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

	//Copy in our FString data into a Char* array
	const int32 InSize = StringIn.GetCharArray().Num();
	char * CharsIn =  new char[InSize];
	for (int i = 0; i < InSize; i++)
		CharsIn[i] = StringIn.GetCharArray()[i];
	
	BYTE *pbDataInput = (BYTE *)CharsIn;
	const DWORD cbDataInput = strlen(CharsIn);
	DataIn.pbData = pbDataInput;
	DataIn.cbData = cbDataInput;
	if (CryptProtectData(
		&DataIn,
		L"EncryptedPayload",
		NULL,
		NULL,
		NULL,
		0,
		&DataOut))
	{
		const int32 OutSize = DataOut.cbData;		
		for (int32 i = 0; i < OutSize; i++)
			Result.AppendChar(DataOut.pbData[i]);
		LocalFree(DataOut.pbData);
	}
	else
	{
		UE_LOG(LogTemp,Display,TEXT("Encryption Failed on windows"));
	}
	delete[] CharsIn;
#endif
	UE_LOG(LogTemp,Display,TEXT("Encrypted Result: %s"),*Result);
	return Result;
}

FString UWindowsEncryptor::Decrypt(const FString& StringIn)
{
	FString Result = "";
	UE_LOG(LogTemp,Display,TEXT("Preparing to decrypt on windows: %s"),*StringIn);
#if PLATFORM_WINDOWS
	DATA_BLOB DataIn;
	DATA_BLOB DataOut;
	
	//Copy in our FString data into a Char* array
	const int32 InSize = StringIn.GetCharArray().Num();
	char * CharsIn =  new char[InSize];
	for (int i = 0; i < InSize; i++)
		CharsIn[i] = StringIn.GetCharArray()[i];
	
	BYTE *pbDataInput = (BYTE *)CharsIn;
	const DWORD cbDataInput = StringIn.GetCharArray().Num();
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
		const int32 OutSize = DataOut.cbData;		
		for (int32 i = 0; i < OutSize; i++)
			Result.AppendChar(DataOut.pbData[i]);
		
		LocalFree(DataOut.pbData);
	}
	else
	{
		UE_LOG(LogTemp,Display,TEXT("Decryption Failed on windows"));
	}
#endif
	UE_LOG(LogTemp,Display,TEXT("Decrypted Result: %s"),*Result);
	return Result;
}
