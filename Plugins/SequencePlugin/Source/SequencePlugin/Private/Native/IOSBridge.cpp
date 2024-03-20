#include "IOSBridge.h"
//#include "MobileNativeCode.h"
//#include <Async/Async.h>
//#include <Engine.h>

#include <iostream>
#include <string>
#include <sstream>
#include <functional>

using namespace std;

// All Objective-C classes are on the path: "MobileNativeCode\Source\MobileNativeCode\Private\IOS\ObjC\"

#if PLATFORM_IOS
/*#include "IOS/Utils/ObjC_Convert.h"

#include "IOS/ObjC/IosHelloWorld.h"
#include "IOS/ObjC/IosAsyncHelloWorld.h"
#include "IOS/ObjC/IosNativeUI.h"
#include "IOS/ObjC/IosExampleArray.h"
#include "IOS/ObjC/IosDeviceInfo.h"*/
#include "Native/IOS/objective_c/IOSOAuth.h"
#endif

void GetIdToken(const FString& providerUrl, void(*Callback)(char *idToken)) 
{
#if PLATFORM_IOS
    let url = providerUrl.GetNSString();
    IOSOAuth *auth = [[IOSOAuth alloc] init];
    [auth loadBrowserWithUrl:url callback:^(char *idToken){
        Callback(idToken);
    }];
#endif
}

void InitiateIosSSO(const FString& providerUrl, UAuthenticator * Callback)
{
    AuthCallback = Callback;
#if PLATFORM_IOS
    //do stuff here
#endif
}
