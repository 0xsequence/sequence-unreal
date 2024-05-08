// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#if PLATFORM_IOS

#import <Foundation/Foundation.h>
#import <Security/Security.h>

@interface IOSEncryptor

- (BOOL) GenerateKeys:();
- (BOOL) LoadKeys:();
- (void) Clean();

- (char *)Encrypt:(NSString *)str;
- (char *)Decrypt:(NSString *)str;

@end
#endif