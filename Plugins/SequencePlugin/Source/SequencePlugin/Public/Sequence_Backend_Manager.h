// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sequence_Backend_Manager.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API ASequence_Backend_Manager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASequence_Backend_Manager();

	UFUNCTION(BlueprintCallable)
		FString Setup();

	UFUNCTION(BlueprintCallable)
		FString Testing_Address_Generation();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
