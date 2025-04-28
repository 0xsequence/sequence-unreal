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
	Checkout->Marketplace = NewObject<USequenceMarketplace>();
	Checkout->Checkout = NewObject<USequenceCheckout>();
	Checkout->Network = ENetwork::PolygonChain;

	UERC1155SaleContract* SaleContract = NewObject<UERC1155SaleContract>();

	SaleContract->ContractAddress = TEXT("0x624ec4ddfb5073b9dd49712036fd56ce895f366e");
	SaleContract->PaymentToken = TEXT("0x3c499c542cEF5E3811e1192ce70d8cC03d5c3359");
	SaleContract->MaxTotal = 2000;
	SaleContract->Data = TEXT("0x0000000000000000000000000000000000000000000000000000000000000000");
	
	FTransakNFTData NFTData;
	NFTData.ImageURL = TEXT("https://dev-metadata.sequence.app/projects/1010/collections/394/tokens/1/image.png");
	NFTData.Name = TEXT("Keaton T-322");
	NFTData.CollectionAddress = TEXT("0x8e9609597f7376098ddfc441f45cae5f37057222");
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

	return true;

}
