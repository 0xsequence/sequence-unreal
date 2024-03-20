#import "IOSOAuth.h"

#if PLATFORM_IOS
#include "IOSAppDelegate.h"
#import <Foundation/Foundation.h>
#import <AuthenticationServices/AuthenticationServices.h>
#endif

static NSString *url = @"";
static NSString *userId = @"";
static Callback completion = ^(char *idToken) {};

typedef void(^Callback)(char *idToken);

@implementation IOSOAuth

+ (NSString *)url {return url;}
+ (NSString *)userId {return userId;}
+ (Callback)completion {return completion;}

+ (IOSOAuth*)GetDelegate {
    static IOSOAuth *Singleton = [[IOSOAuth alloc] init];
    return Singleton;
}

- (ASPresentationAnchor)presentationAnchorForAuthorizationController:(ASAuthorizationController *)controller {
    return UIApplication.sharedApplication.keyWindow;
}

- (void)loadBrowserWithUrl: (NSString *)providerUrl callback:(void(^)(char *))callback {
    url = providerUrl;
    completion = [callback copy];
    [[IOSOAuth GetDelegate] loadBrowserURLInIOSThread];
}

- (void)loadBrowserURLInIOSThread {
    NSURL *_url = [NSURL URLWithString:[IOSOAuth url]];
    
    ASAuthorizationAppleIDProvider *appleIDProvider = [ASAuthorizationAppleIDProvider new];
    ASAuthorizationAppleIDRequest *request = appleIDProvider.createRequest;
    request.requestedScopes = @[ASAuthorizationScopeFullName, ASAuthorizationScopeEmail];
    ASAuthorizationController *controller = [[ASAuthorizationController alloc] initWithAuthorizationRequests:@[request]];
    
    controller.delegate = self;
    controller.presentationContextProvider = self;
    
    [controller performRequests];
}

- (void)authorizationController:(ASAuthorizationController *)controller didCompleteWithAuthorization:(ASAuthorization *)authorization {
    if ([authorization.credential isKindOfClass:[ASAuthorizationAppleIDCredential class]]) {
        ASAuthorizationAppleIDCredential *appleIDCredential = authorization.credential;
        NSString *user = appleIDCredential.user;
        
        char *idToken = [[IOSOAuth GetDelegate] ConvertNSStringToChars:appleIDCredential.user];
        completion(idToken);
    }
}

- (void)authorizationController:(ASAuthorizationController *)controller didCompleteWithError:(NSError *)error {
    NSString *errorMsg = nil;
    switch (error.code) {
        case ASAuthorizationErrorCanceled:
            errorMsg = @"ASAuthorizationErrorCanceled";
            break;
        case ASAuthorizationErrorFailed:
            errorMsg = @"ASAuthorizationErrorFailed";
            break;
        case ASAuthorizationErrorInvalidResponse:
            errorMsg = @"ASAuthorizationErrorInvalidResponse";
            break;
        case ASAuthorizationErrorNotHandled:
            errorMsg = @"ASAuthorizationErrorNotHandled";
            break;
        case ASAuthorizationErrorUnknown:
            errorMsg = @"ASAuthorizationErrorUnknown";
            break;
    }
    
    if (errorMsg) {
        NSLog(@"errorMsg: %@", errorMsg);
        return;
    }
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self name:ASAuthorizationAppleIDProviderCredentialRevokedNotification object:nil];
    [super dealloc];
}

- (char *)ConvertNSStringToChars:(NSString *)str {
    const char *strChars = [str UTF8String];
    return (char*)strChars;
}
@end
