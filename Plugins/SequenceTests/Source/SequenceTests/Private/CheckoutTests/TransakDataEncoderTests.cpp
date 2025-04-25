// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "SequencePlugin/Public/Checkout/Transak/Structs/TransakNFTData.h"
#include "SequencePlugin/Public/Checkout/Transak/TransakNFTDataEncoder.h"
#include "SequencePlugin/Public/Checkout/Transak/CallDataCompressor.h"


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTransakNFTDataEncoderTests, "SequencePlugin.EndToEnd.CheckoutTests.Transak.NFTDataEncoder", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTransakNFTDataEncoderTests::RunTest(const FString& Parameters)
{
    FString ImageURL = TEXT("https://dev-metadata.sequence.app/projects/1010/collections/394/tokens/1/image.png");
    FString Name = TEXT("Keaton T-322");
    FString CollectionAddress = TEXT("0xdeb398f41ccd290ee5114df7e498cf04fac916cb");
    TArray<FString> TokenIDs = { TEXT("1") };
    TArray<float> Prices = { 0.02f };
    int32 Quantity = 1;

    FTransakNFTData Data(ImageURL, Name, CollectionAddress, TokenIDs, Prices, Quantity, ENFTType::ERC1155);

    FString EncodedData = FTransakNFTDataEncoder::Encode(Data);

    FString Expected = TEXT("W3siaW1hZ2VVUkwiOiJodHRwczovL2Rldi1tZXRhZGF0YS5zZXF1ZW5jZS5hcHAvcHJvamVjdHMvMTAxMC9jb2xsZWN0aW9ucy8zOTQvdG9rZW5zLzEvaW1hZ2UucG5nIiwibmZ0TmFtZSI6IktlYXRvbiBULTMyMiIsImNvbGxlY3Rpb25BZGRyZXNzIjoiMHhkZWIzOThmNDFjY2QyOTBlZTUxMTRkZjdlNDk4Y2YwNGZhYzkxNmNiIiwidG9rZW5JRCI6WyIxIl0sInByaWNlIjpbMC4wMl0sInF1YW50aXR5IjoxLCJuZnRUeXBlIjoiRVJDMTE1NSJ9XQ%3D%3D");

    TestEqual(TEXT("Encoded NFT Data matches expected output"), EncodedData, Expected);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTestCallDataCompressor, "SequencePlugin.EndToEnd.CheckoutTests.Transak.CallDataCompressor", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTestCallDataCompressor::RunTest(const FString& Parameters)
{
    FString Calldata = TEXT("0x60e606f60000000000000000000000004a598b7ec77b1562ad0df7dc64a162695ce4c78a00000000000000000000000000000000000000000000000000000000000000e0000000000000000000000000000000000000000000000000000000000000012000000000000000000000000000000000000000000000000000000000000001600000000000000000000000003c499c542cef5e3811e1192ce70d8cc03d5c33590000000000000000000000000000000000000000000000000000000000004e2000000000000000000000000000000000000000000000000000000000000001a000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000");

    FString Compressed = FCallDataCompressor::Compress(Calldata);

    FString Expected = TEXT("eJztkMsNxDAIBVsC8zPlOIBrSPmbBiytxCGXzI3DvAfArVAKuhUO8BKfl1WYXSg6VkJuy1BeqENdojhsrpP%2FH9XTcTT94%2FlAwe4hPKK2FE3EQvRnMsgZAZQSROKdeq7u%2Fs33Pwmf%2Fyrt%2Fh9Zdb5R");

    TestEqual(TEXT("Compressed calldata matches expected output"), Compressed, Expected);

    return true;
}