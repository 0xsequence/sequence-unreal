// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SequenceWalletBPTypes.h"
#include "SequenceWalletBP.generated.h"

//ASYNC Response Delegates//

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIIndexerPing, FSequenceResponseStatus, ResponseStatus, bool, PingResponse);

//ASYNC Response Delegates//

/**
 * Used for exposing portions of the USequenceWallet
 * to Blueprints for ease of use.
 */
UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceWalletBP : public UObject
{
	GENERATED_BODY()
private:
	
public:
	USequenceWalletBP();

	//ASYNC Response Bindable Delegates//

	UPROPERTY(BlueprintAssignable, Category="ASYNC_RESPONSE")
	FOnIIndexerPing OnIndexerPing;
	
	//ASYNC Response Bindable Delegates//
private:

	//ASYNC Response Internal Calls//

	void CallOnIndexerPing(const FSequenceResponseStatus& Status, bool PingResponse) const;
	
	//ASYNC Response Internal Calls//
	
public:
	//SYNC//
	
	UFUNCTION(BlueprintCallable)
	FString GetWalletAddress();

	UFUNCTION(BlueprintCallable)
	int64 GetNetworkId();

	UFUNCTION(BlueprintCallable)
	void UpdateNetworkId(int64 NewNetworkId);

	UFUNCTION(BlueprintCallable)
	void UpdateProviderUrl(const FString& NewProviderUrl);
	
	UFUNCTION(BlueprintCallable)
	void OpenTransakUrl();

	UFUNCTION(BlueprintCallable)
	void SignOut();
	
	//SYNC//
	
	//ASYNC//

	UFUNCTION(BlueprintCallable)
	void IndexerPing();
	
	//ASYNC//
};
