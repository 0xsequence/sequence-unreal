#include "IOSBridge.h"
#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include "Native/IOS/objective_c/IOSOAuth.h"

using namespace std;

UIOSBridge::UIOSBridge()
{
    
}


void UIOSBridge::InitiateIosSSO(const FString& providerUrl, void(*IOSCallback)(char *idToken))
{
#if PLATFORM_IOS
	NSString * url = providerUrl.GetNSString();
	IOSOAuth *auth = [[IOSOAuth alloc] init];
	[auth loadBrowserWithUrl:url callback:^(char *idToken){
		IOSCallback(idToken);
	}];
#endif
}
