// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#import "NativeAppleEncryptor.h"
#import <Foundation/Foundation.h>
#import <Security/Security.h>

static SecKeyRef privateKey;
static SecKeyRef publicKey;
static NSString * ErrorCapture = @"";

@implementation NativeAppleEncryptor

+ (NativeAppleEncryptor*)GetDelegate {
    static NativeAppleEncryptor *Singleton = [[NativeAppleEncryptor alloc] init];
    return Singleton;
}

- (BOOL) GenerateKeys 
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
        return false;
    }
    publicKey = SecKeyCopyPublicKey(privateKey);
    return true;
}

- (BOOL) LoadKeys 
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
        ErrorCapture = @"Private key retrieved successfully";
        publicKey = SecKeyCopyPublicKey(privateKey);
        return true;
    }
    else if (status == errSecItemNotFound)
    {
        NSLog(@"Private key not found. Add it if needed.");
        ErrorCapture = @"Private key not found. Add it if needed.";
        return [self GenerateKeys];
    }
    else
    {
        NSLog(@"Keychain error: %ld", (long)status);
        ErrorCapture = @"Keychain error";
        return false;
    }
}

- (void) Clean
{
    if (publicKey)  { CFRelease(publicKey);  }
    if (privateKey) { CFRelease(privateKey); }
}

- (char *)Encrypt:(NSString *)str
{
    char *EncryptedChars = nullptr;
    if ([self LoadKeys])
    {
        CFDataRef plainText = (__bridge CFDataRef)[str dataUsingEncoding:NSUTF8StringEncoding];
        CFErrorRef error = NULL;
        
        CFDataRef EncryptedData = SecKeyCreateEncryptedData(
        publicKey,
        kSecKeyAlgorithmRSAEncryptionPKCS1,
        plainText,
        &error);
        
        NSData * PreProcEncryptedData = (__bridge NSData *)EncryptedData;
        NSString * EncryptedDataString = [[NSString alloc] initWithData:PreProcEncryptedData encoding:NSUTF8StringEncoding];
        EncryptedChars = [[NativeAppleEncryptor GetDelegate] ConvertNSStringToChars:EncryptedDataString];
    }
    else
    {//Failure state
        NSLog(@"Failed to load encryption key");
        EncryptedChars = [[NativeAppleEncryptor GetDelegate] ConvertNSStringToChars:ErrorCapture];
    }
    [self Clean];
    return EncryptedChars;
}

- (char *)Decrypt:(NSString *)str
{
    char *DecryptedChars = nullptr;
    if ([self LoadKeys])
    {
            CFDataRef plainText = (__bridge CFDataRef)[str dataUsingEncoding:NSUTF8StringEncoding];
            CFErrorRef error = NULL;
            
            CFDataRef DecryptedData = SecKeyCreateDecryptedData(
            privateKey,
            kSecKeyAlgorithmRSAEncryptionPKCS1,
            plainText,
            &error);
            
            NSData * PreProcDecryptedData = (__bridge NSData *)DecryptedData;
            NSString * DecryptedDataString = [[NSString alloc] initWithData:PreProcDecryptedData encoding:NSUTF8StringEncoding];
            DecryptedChars = [[NativeAppleEncryptor GetDelegate] ConvertNSStringToChars:DecryptedDataString];
    }
    else
    {//Failure state
        NSLog(@"Failed to load decryption key");
        NSString * FailureString = @"Failed to load decryption key";
        DecryptedChars = [[NativeAppleEncryptor GetDelegate] ConvertNSStringToChars:FailureString];
    }
    [self Clean];
    return DecryptedChars;
}

- (char *)ConvertNSStringToChars:(NSString *)str {
    const char *strChars = [str UTF8String];
    return (char*)strChars;
}
@end