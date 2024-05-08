// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#import "IOSEncryptor.h"
#import <Foundation/Foundation.h>
#import <Security/Security.h>

static SecKeyRef privateKey;
static SecKeyRef publicKey;

@implementation IOSEncryptor

- (BOOL) GenerateKeys:()
{
    NSData* tag = [@"com.Sequence.keys.Main" dataUsingEncoding:NSUTF8StringEncoding];
    
    NSDictionary* attributes =
        @{ (id)kSecAttrKeyType:               (id)kSecAttrKeyTypeRSA,
           (id)kSecAttrKeySizeInBits:         @2048,
           (id)kSecPrivateKeyAttrs:
               @{ (id)kSecAttrIsPermanent:    @YES,
                  (id)kSecAttrApplicationTag: tag,
                  },   
         };
    CFErrorRef error = NULL;
    privateKey = SecKeyCreateRandomKey((__bridge CFDictionaryRef)attributes,&error);
    if (!privateKey) {
        NSError *err = CFBridgingRelease(error);  // ARC takes ownership
        return @NO;
    }
    publicKey = SecKeyCopyPublicKey(privateKey);
    return @YES;
}

- (BOOL) LoadKeys:()
{
    NSDictionary *query = @ {
        (__bridge id)kSecClass: (__bridge id)kSecClassKey,
        (__bridge id)kSecAttrApplicationTag: @"com.Sequence.keys.Main",
        (__bridge id)kSecReturnRef: @YES,
    };
    
    SecKeyRef privateKeyRef = NULL;
    OSStatus status = SecItemCopyMatching((__bridge CFDictionaryRef)query, (CFTypeRef *)&privateKey);
    if (status == errSecSuccess) 
    {
        NSLog(@"Private key retrieved successfully.");
        publicKey = SecKeyCopyPublicKey(privateKey);
        return @YES;
    }
    else if (status == errSecItemNotFound)
    {
        NSLog(@"Private key not found. Add it if needed.");
        return [self GenerateKeys];
    }
    else
    {
        NSLog(@"Keychain error: %ld", (long)status);
        return @NO;
    }
}

- (void) Clean()
{
    if (publicKey)  { CFRelease(publicKey);  }
    if (privateKey) { CFRelease(privateKey); }
}

- (char *)Encrypt:(NSString *)str
{
    if ([self LoadKeys])
    {
        CFDataRef plainText = (__bridge CFDataRef)[str dataUsingEncoding:NSUTF8StringEncoding];
        CFErrorRef *error;
        
        CFDataRef EncryptedData = SecKeyCreateEncryptedData(
        publicKey,
        kSecKeyAlgorithmRSAEncryptionPKCS1,
        plainText,
        &error);
        
        [self Clean];
        //now we need to convert the CFDataRef to NSString NSString to char * and return that!
    }
    else
    {
        //In the event we get here it means no keys could be loaded / generated something is very wrong
        //print an error log and return an empty string
    }
}

- (char *)Decrypt:(NSString *)str
{
    if ([self LoadKeys])
    {
            CFDataRef plainText = (__bridge CFDataRef)[str dataUsingEncoding:NSUTF8StringEncoding];
            CFErrorRef *error;
            
            CFDataRef EncryptedData = SecKeyCreateDecryptedData(
            privateKey,
            kSecKeyAlgorithmRSAEncryptionPKCS1,
            plainText,
            &error);
            [self Clean];
    }
    else
    {
        //In the event we get here it means no keys could be loaded / generated something is very wrong
        //print an error log and return an empty string
    }
}

@end