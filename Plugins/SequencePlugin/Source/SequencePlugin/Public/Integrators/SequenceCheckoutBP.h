#pragma once

#include "CoreMinimal.h"
#include "Checkout/Checkout.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SequenceCheckoutBP.generated.h"

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceCheckoutBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	
public:
	USequenceCheckoutBP();
	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK/Checkout")
	UCheckout* GetCheckout() const;

private:
	UCheckout* Checkout;
	
};
