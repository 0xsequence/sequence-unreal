#include "Misc/AutomationTest.h"
#include "SequencePlugin/Public/Checkout/Transak/TransakCheckout.h"
#include "SequencePlugin/Public/Sequence/SequenceWallet.h"
#include "SequencePlugin/Public/Marketplace/SequenceMarketplace.h"
#include "SequencePlugin/Public/Checkout/SequenceCheckout.h"
#include "SequencePlugin/Public/Types/ERC1155SaleContract.h"


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTransakCheckoutLinkPrimarySaleTest, "SequencePlugin.EndToEnd.CheckoutTests.Transak.PrimarySaleERC1155", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTransakCheckoutLinkPrimarySaleTest::RunTest(const FString& Parameters)
{
	UTransakCheckout* Checkout = NewObject<UTransakCheckout>();
	Checkout->WalletAddress = FString(TEXT("0xD2eFbb2f18bfE3D265b26D2ACe83400A65335a07"));
	Checkout->Marketplace = NewObject<USequenceMarketplace>();
	Checkout->Checkout = NewObject<USequenceCheckout>();
	Checkout->Network = ENetwork::ArbitrumOne;

	UERC1155SaleContract* SaleContract = NewObject<UERC1155SaleContract>();

	SaleContract->ContractAddress = TEXT("0x624ec4ddfb5073b9dd49712036fd56ce895f366e");
	SaleContract->PaymentToken = TEXT("0xf3C3351D6Bd0098EEb33ca8f830FAf2a141Ea2E1");
	SaleContract->MaxTotal = 1;
	SaleContract->Data = TEXT("0x0000000000000000000000000000000000000000000000000000000000000000");


	FTransakNFTData NFTData;
	NFTData.ImageURL = TEXT("https://metadata.sequence.app/projects/158/collections/1233/tokens/0/image.png");
	NFTData.Name = TEXT("Icy Longsword");
	NFTData.CollectionAddress = TEXT("0x8e9609597f7376098ddfc441f45cae5f37057222");
	NFTData.TokenID = { TEXT("1") };
	NFTData.Price = { 0 };
	NFTData.Quantity = 1;
	NFTData.Type = ENFTType::ERC1155;

	FTransakContractId ContractId;
	ContractId.Id = "66c5a2cf2fb1688e11fcb167";
	ContractId.ContractAddress = "0xB537a160472183f2150d42EB1c3DD6684A55f74c";
	ContractId.Chain = UEnum::GetValueAsString(ENetwork::ArbitrumOne);
	ContractId.PriceTokenSymbol = "USDC";

	TArray<uint8> data;   // Empty Data
	TArray<FString> proof;
	
	FString Link = Checkout->BuildNFTCheckoutLinkFromERC1155(SaleContract, NFTData, ContractId, data, proof);

	TestFalse(TEXT("Failed"), Link.IsEmpty());

	return true;

}
