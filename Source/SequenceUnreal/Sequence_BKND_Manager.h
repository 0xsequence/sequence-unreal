// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Engine/GameEngine.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sequence_BKND_Manager.generated.h"

UCLASS()

class SEQUENCEUNREAL_API ASequence_BKND_Manager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASequence_BKND_Manager();

	UFUNCTION(BlueprintCallable)
		int32 Setup(int32 setup_prop);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};