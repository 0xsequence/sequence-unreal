#include "Integrators/SequenceCheckoutBP.h"

void USequenceCheckoutBP::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Warning, TEXT("SequenceCheckoutBP::Initialize"));
}

void USequenceCheckoutBP::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogTemp, Warning, TEXT("SequenceCheckoutBP::Deinitialize"));
}

bool USequenceCheckoutBP::ShouldCreateSubsystem(UObject* Outer) const
{
	if (this->GetClass()->IsInBlueprint() && Super::ShouldCreateSubsystem(Outer))
	{
		return true;
	}
	
	return false;
}

USequenceCheckoutBP::USequenceCheckoutBP()
{
	this->Checkout = NewObject<UCheckout>();
}

UCheckout* USequenceCheckoutBP::GetCheckout() const
{
	return this->Checkout;
}
