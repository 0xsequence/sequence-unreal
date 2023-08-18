// Fill out your copyright notice in the Description page of Project Settings.


#include "SystemDataBuilder.h"
#include "Indexer.h"
#include "Syncer.h"
#include "Sequence/SequenceAPI.h"


/*
* We expect to receive an authable wallet, a proper chainId, and PublicAddress and a valid indexer
*/
void USystemDataBuilder::initBuildSystemData(UIndexer* indexer, SequenceAPI::FSequenceWallet* wallet, int64 chainId, FString publicAddress)
{
	//Create a syncer for request management
	USyncer* syncer = NewObject<USyncer>();

	const TSuccessCallback<FGetTokenBalancesReturn> GenericSuccess = [](const FGetTokenBalancesReturn tokenBalances)
	{
		//need a token balances response parser!
	};

	const FFailureCallback GenericFailure = [](const SequenceError Error)
	{

	};
	
	//done creating datums for request management

	FGetTokenBalancesArgs args;
	indexer->GetTokenBalances(chainId, args, GenericSuccess, GenericFailure);

	this->systemData.user_data.public_address = publicAddress;
}

FSystemData_BE USystemDataBuilder::getSystemData()
{
	return this->systemData;
}