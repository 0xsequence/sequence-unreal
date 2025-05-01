#include "Misc/AutomationTest.h"
#include "SequencePlugin/Public/Checkout/Transak/TransakCheckout.h"
#include "SequencePlugin/Public/Sequence/SequenceWallet.h"
#include "SequencePlugin/Public/Marketplace/SequenceMarketplace.h"
#include "SequencePlugin/Public/Checkout/SequenceCheckout.h"
#include "SequencePlugin/Public/Types/ERC1155SaleContract.h"
#include "SequencePlugin/Private/Provider.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTransakCheckoutLinkPrimarySaleTest, "SequencePlugin.EndToEnd.CheckoutTests.Transak.PrimarySaleERC1155", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTransakCheckoutLinkPrimarySaleTest::RunTest(const FString& Parameters)
{
	UTransakCheckout* Checkout = NewObject<UTransakCheckout>();
	Checkout->WalletAddress = FString(TEXT("0xD2eFbb2f18bfE3D265b26D2ACe83400A65335a07"));
	Checkout->Network = ENetwork::PolygonChain;

	UERC1155SaleContract* SaleContract = NewObject<UERC1155SaleContract>();

	SaleContract->ContractAddress = TEXT("0xe65b75eb7c58ffc0bf0e671d64d0e1c6cd0d3e5b");
	SaleContract->PaymentToken = TEXT("0x3c499c542cEF5E3811e1192ce70d8cC03d5c3359");
	SaleContract->MaxTotal = 20000;
	SaleContract->Data = TEXT("Minted using the Sequence Unreal SDK");
	
	FTransakNFTData NFTData;
	NFTData.ImageURL = TEXT("https://dev-metadata.sequence.app/projects/1010/collections/394/tokens/1/image.png");
	NFTData.Name = TEXT("Keaton T-322");
	NFTData.CollectionAddress = TEXT("0xdeb398f41ccd290ee5114df7e498cf04fac916cb");
	NFTData.TokenID = { TEXT("1") };
	NFTData.Price = { 0.02 };
	NFTData.Quantity = 1;
	NFTData.Type = ENFTType::ERC1155;

	FTransakContractId ContractId;
	ContractId.Id = "674eb5613d739107bbd18ed2";
	ContractId.ContractAddress = "0xe65b75eb7c58ffc0bf0e671d64d0e1c6cd0d3e5b";
	ContractId.Chain = UEnum::GetValueAsString(ENetwork::PolygonChain);
	ContractId.PriceTokenSymbol = "USDC";

	TArray<uint8> data;
	TArray<FString> proof;
	
	FString Link = Checkout->BuildNFTCheckoutLinkFromERC1155(SaleContract, NFTData, ContractId, data, proof);

	TestFalse(TEXT("Failed"), Link.IsEmpty());

	FString ExpectedLink = TEXT("https://global.transak.com/?apiKey=5911d9ec-46b5-48fa-a755-d59a715ff0cf&isNFT=true&calldata=eJzlj8kRAzEIBFPiHKRwtAjF4PCt9X%2BrXNbT82KA5qAXqEBYoAfZ8N6uqIy42CFj0lwxEzYYgu5ZltHGE%2F%2Bd6gxnOeQf3ydN6z3dJGt5aWMu5r5d0GyZpNNT1fvJeqvT%2B%2FOM3xP%2BmxeziY4Kg8OEwkNvj9ixocGFXOHB4bujoJ%2FMjkK2Z%2BRdN7Pr8I%2Bf9Qb63cD0&contractId=674eb5613d739107bbd18ed2&cryptoCurrencyCode=USDC&estimatedGasLimit=500000&nftData=W3siaW1hZ2VVUkwiOiJodHRwczovL2Rldi1tZXRhZGF0YS5zZXF1ZW5jZS5hcHAvcHJvamVjdHMvMTAxMC9jb2xsZWN0aW9ucy8zOTQvdG9rZW5zLzEvaW1hZ2UucG5nIiwibmZ0TmFtZSI6IktlYXRvbiBULTMyMiIsImNvbGxlY3Rpb25BZGRyZXNzIjoiMHhkZWIzOThmNDFjY2QyOTBlZTUxMTRkZjdlNDk4Y2YwNGZhYzkxNmNiIiwidG9rZW5JRCI6WyIxIl0sInByaWNlIjpbMC4wMl0sInF1YW50aXR5IjoxLCJuZnRUeXBlIjoiRVJDMTE1NSJ9XQ%3D%3D&walletAddress=0xD2eFbb2f18bfE3D265b26D2ACe83400A65335a07&disableWalletAddressForm=true&partnerOrderId=0xD2eFbb2f18bfE3D265b26D2ACe83400A65335a07-");
	TestTrue(TEXT("TransakNFTCheckoutLink starts with expectedLink"), Link.StartsWith(ExpectedLink));


	return true;

}
