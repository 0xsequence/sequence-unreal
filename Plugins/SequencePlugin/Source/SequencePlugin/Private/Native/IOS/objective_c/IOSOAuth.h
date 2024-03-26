#pragma once

#if PLATFORM_IOS
#import <Foundation/Foundation.h>
#import <AuthenticationServices/AuthenticationServices.h>

typedef void(^Callback)(char *idToken);

@interface IOSOAuth : UIViewController <ASAuthorizationControllerDelegate, ASAuthorizationControllerPresentationContextProviding>
+ (NSString *)url;
+ (NSString *)userId;
+ (Callback)completion;
+ (IOSOAuth*)GetDelegate;
- (void)loadBrowserWithUrl:(NSString *)cID nonce:(NSString *)nonce callback:(void(^)(char *))callback;
- (void)loadBrowserURLInIOSThread: (NSString *)clientID nonce:(NSString *)nonce;
- (ASPresentationAnchor)presentationAnchorForAuthorizationController: (ASAuthorizationController *)controller;
- (char *)ConvertNSStringToChars:(NSString *)str;
@end
#endif
