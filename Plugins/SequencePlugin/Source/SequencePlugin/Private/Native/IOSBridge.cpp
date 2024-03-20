#include "IOSBridge.h"
#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include "Native/IOS/objective_c/IOSOAuth.h"

using namespace std;

void InitiateIosSSO(const FString& providerUrl, UAuthenticator * Callback)
{
    AuthBridgeCallback = Callback;
#if PLATFORM_IOS
    let url = providerUrl.GetNSString();
    IOSOAuth *auth = [[IOSOAuth alloc] init];
    [auth loadBrowserWithUrl:url callback:^(char *idToken){
        FString parsedToken(idToken);
        AuthBridgeCallback->SocialLogin(Test);
    }];
#endif
}
