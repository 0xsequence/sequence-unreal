#import "IOSOAuth.h"
#include "IOSAppDelegate.h"
#import <Foundation/Foundation.h>
#import <AuthenticationServices/AuthenticationServices.h>

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

-(void)InitGoogleSignin:(NSString *)URL Redirect:(NSString *)RedirectUri callback:(void(^)(char *))callback {
 completion = [callback copy];
 
 NSURL * _url = [NSURL URLWithString:URL];
 NSURL * _redirect = [NSURL URLWithString:RedirectUri];
 
 ASAuthorizationSingleSignOnProvider *authProvider = [[ASAuthorizationSingleSignOnProvider alloc] initWithIdentityProvider:_url];
 ASAuthorizationSingleSignOnRequest *request = authProvider.createRequest;
 //might need to do nonce assignments here
 request.requestedScopes = @[ASAuthorizationScopeFullName, ASAuthorizationScopeEmail];
 ASAuthorizationController *controller = [[ASAuthorizationController alloc] initWithAuthorizationRequests:@[request]];
 controller.delegate = self;
 controller.presentationContextProvider = self;    
 [controller performRequests];
}

- (void)loadBrowserWithUrl:(NSString *)cID nonce:(NSString *)nonce callback:(void(^)(char *))callback {
    completion = [callback copy];
    [[IOSOAuth GetDelegate] loadBrowserURLInIOSThread:cID nonce:nonce];
}

- (void)loadBrowserURLInIOSThread: (NSString *)clientID nonce:(NSString *)nonce {
    NSURL *_url = [NSURL URLWithString:[IOSOAuth url]];
    
    ASAuthorizationAppleIDProvider *appleIDProvider = [ASAuthorizationAppleIDProvider new];
    ASAuthorizationAppleIDRequest *request = appleIDProvider.createRequest;
    request.nonce = nonce;
    request.state = clientID;
    request.requestedScopes = @[ASAuthorizationScopeFullName, ASAuthorizationScopeEmail];
    ASAuthorizationController *controller = [[ASAuthorizationController alloc] initWithAuthorizationRequests:@[request]];
    
    controller.delegate = self;
    controller.presentationContextProvider = self;
    
    [controller performRequests];
    
//     ASAuthorizationSingleSignOnProvider *authProvider = [ASAuthorizationSingleSignOnProvider authorizationProviderWithIdentityProviderURL:_url];    
//         BOOL canPerformAuthorization = authProvider.canPerformAuthorization;
//         if (canPerformAuthorization) {
//             ASAuthorizationSingleSignOnRequest *request = [authProvider createRequest];
//             ASAuthorizationController *controller = [[ASAuthorizationController alloc] initWithAuthorizationRequests:@[request]];
//             controller.delegate = self;
//             controller.presentationContextProvider = self;
//             [controller performRequests];
//         } else {
//             NSLog(@"Authorization not supported for this provider.");
//         }
}



- (void)authorizationController:(ASAuthorizationController *)controller didCompleteWithAuthorization:(ASAuthorization *)authorization {
    if ([authorization.credential isKindOfClass:[ASAuthorizationAppleIDCredential class]]) {
        ASAuthorizationAppleIDCredential *appleIDCredential = authorization.credential;
        
        NSString *token = [[NSString alloc] initWithData:appleIDCredential.identityToken encoding:NSUTF8StringEncoding];
        char *idToken = [[IOSOAuth GetDelegate] ConvertNSStringToChars:token];
        completion(idToken);
    }
    
    if ([authorization.credential isKindOfClass:[ASAuthorizationSingleSignOnCredential class]]) {
        ASAuthorizationSingleSignOnCredential * credential = authorization.credential;
        
        NSString * token = [[NSString alloc] initWithData:credential.identityToken encoding:NSUTF8StringEncoding];
        char *idToken = [[IOSOAuth GetDelegate] ConvertNSStringToChars:token];
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
