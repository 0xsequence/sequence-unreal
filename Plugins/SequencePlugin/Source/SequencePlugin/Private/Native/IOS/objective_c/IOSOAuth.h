// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#if PLATFORM_IOS
#import <Foundation/Foundation.h>
#import <AuthenticationServices/AuthenticationServices.h>

typedef void(^Callback)(char *idToken);

@interface IOSOAuth : UIViewController <ASAuthorizationControllerDelegate, ASAuthorizationControllerPresentationContextProviding, ASWebAuthenticationPresentationContextProviding>
+ (NSString *)url;
+ (NSString *)userId;
+ (Callback)completion;
+ (IOSOAuth*)GetDelegate;
- (void)loadBrowserWithUrl:(NSString *)cID callback:(void(^)(char *))callback;
- (void)loadBrowserURLInIOSThread: (NSString *)clientID;
- (ASPresentationAnchor)presentationAnchorForAuthorizationController: (ASAuthorizationController *)controller;
- (ASPresentationAnchor)presentationAnchorForWebAuthenticationSession:(ASWebAuthenticationSession *)session;
- (char *)ConvertNSStringToChars:(NSString *)str;

//Google Signin
-(void)InitGoogleSignin:(NSString *)URL Scheme:(NSString *)Scheme callback:(void(^)(char *))callback;


@end
#endif
