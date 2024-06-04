// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#if PLATFORM_IOS || PLATFORM_MAC

#import <Foundation/Foundation.h>
#import <Security/Security.h>

@interface NativeAppleEncryptor : NSObject

+ (NativeAppleEncryptor*)GetDelegate;
- (BOOL) GenerateKeys;
- (BOOL) LoadKeys;
- (void) Clean;
- (char *)ConvertNSStringToChars:(NSString *)str;

- (char *)Encrypt:(NSString *)str;
- (char *)Decrypt:(NSString *)str;

@end
#endif