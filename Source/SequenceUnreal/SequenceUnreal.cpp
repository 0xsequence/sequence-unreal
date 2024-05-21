// Copyright Epic Games, Inc. All Rights Reserved.

#include "SequenceUnreal.h"
#include "Modules/ModuleManager.h"
#include "ABI/ABI.h"
#include "Sequence/SequenceAPI.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, SequenceUnreal, "SequenceUnreal" );

void Test()
{
	const TSuccessCallback<FGetTransactionHistoryReturn> GenericSuccess = [=](const FGetTransactionHistoryReturn& transactionHistory)
	{
		//Response is in FGetTransactionHistoryReturn
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError& Error)
	{
		//GetTransactionHistory Failure
	};
   
	const FCredentials_BE Credentials;//Replace this var with your own credentials however you choose to get them
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Api = WalletOptional.GetValue();

		FGetTransactionHistoryArgs args;
		args.filter.accountAddress = Api->GetWalletAddress();
		args.includeMetaData = true;
		args.page.page = 0;
		args.page.more = true;    

		Api->GetTransactionHistory(args, GenericSuccess, GenericFailure);
	}
}