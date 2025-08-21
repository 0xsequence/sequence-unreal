#include "EcosystemWallet/SequenceConnectBP.h"

USequenceConnectBP::USequenceConnectBP()
{
	this->SequenceConnect = NewObject<USequenceConnect>();
}

void USequenceConnectBP::SignInWithGoogle()
{
	this->SequenceConnect->SignInWithGoogle();
}

USequenceConnect* USequenceConnectBP::GetSequenceConnect() const
{
	return this->SequenceConnect;
}

