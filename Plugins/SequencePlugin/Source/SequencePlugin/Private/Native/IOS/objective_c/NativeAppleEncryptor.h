

#pragma once

#if PLATFORM_IOS || PLATFORM_MAC

#import <Foundation/Foundation.h>
#import <Security/Security.h>

@interface NativeAppleEncryptor : NSObject

- (BOOL) GenerateKeys;
- (BOOL) LoadKeys;
- (void) Clean;
- (char *)ConvertNSStringToChars:(NSString *)str;

- (char *)Encrypt:(NSString *)str;
- (char *)Decrypt:(NSString *)str;

@end
#endif