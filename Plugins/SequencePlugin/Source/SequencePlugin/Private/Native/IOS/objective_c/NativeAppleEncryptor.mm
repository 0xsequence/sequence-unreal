// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#import "NativeAppleEncryptor.h"
#import <Foundation/Foundation.h>
#import <Security/Security.h>

static SecKeyRef privateKey;
static SecKeyRef publicKey;
static NSString * ErrorCapture = @"";

@implementation NativeAppleEncryptor

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
        printf("Failed to generate private key\n");
        return false;
    }
    publicKey = SecKeyCopyPublicKey(privateKey);
    printf("successfully generated private key\n");
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
        ErrorCapture = @"Private key retrieved successfully";
        printf("SecSuccess\n");
        publicKey = SecKeyCopyPublicKey(privateKey);
        return true;
    }
    else if (status == errSecItemNotFound)
    {
        ErrorCapture = @"Private key not found. Add it if needed.";
        printf("ErrSecItemNotFound\n");
        return [self GenerateKeys];
    }
    else
    {
        //NSLog(@"Keychain error: %ld", (long)status);
        ErrorCapture = @"Keychain error";
        printf("KeyChain Error\n");
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
    NSLog(@"Input to encrypt: %@", str);
    if ([self LoadKeys])
    {
        CFDataRef plainText = (__bridge CFDataRef)[str dataUsingEncoding:NSUTF8StringEncoding];
        CFErrorRef error = NULL;
        
        CFDataRef EncryptedData = SecKeyCreateEncryptedData(
        publicKey,
        kSecKeyAlgorithmRSAEncryptionOAEPSHA512AESGCM,
        plainText,
        &error);
        
        if (error)
        {
            NSError *err = CFBridgingRelease(error);
            NSLog(@"Captured Error: %@", err);
        }
        
        NSData * PreProcEncryptedData = (__bridge NSData *)EncryptedData;
        NSString * EncryptedDataString = [[NSString alloc] initWithData:PreProcEncryptedData encoding:NSUTF8StringEncoding];
        NSLog(@"Encrypted Data: %@", EncryptedDataString);
        char * EncryptedChars = [self ConvertNSStringToChars:EncryptedDataString];
        [self Clean];
        return EncryptedChars;
    }
    else
    {//Failure state
        printf("Failed to load encryption key\n");
        char * ErrorChars = [self ConvertNSStringToChars:ErrorCapture];
        [self Clean];
        return ErrorChars;
    }
}

- (char *)Decrypt:(NSString *)str
{
    NSLog(@"Input to decrypt: %@", str);
    if ([self LoadKeys])
    {
            CFDataRef plainText = (__bridge CFDataRef)[str dataUsingEncoding:NSUTF8StringEncoding];
            CFErrorRef error = NULL;
            
            CFDataRef DecryptedData = SecKeyCreateDecryptedData(
            privateKey,
            kSecKeyAlgorithmRSAEncryptionOAEPSHA512AESGCM,
            plainText,
            &error);
            
            if (error)
            {
                NSError *err = CFBridgingRelease(error);
                NSLog(@"Captured Error: %@", err);
            }
            
            NSData * PreProcDecryptedData = (__bridge NSData *)DecryptedData;
            NSString * DecryptedDataString = [[NSString alloc] initWithData:PreProcDecryptedData encoding:NSUTF8StringEncoding];
            NSLog(@"Decrypted Data: %@", DecryptedDataString);
            char * DecryptedChars = [self ConvertNSStringToChars:DecryptedDataString];
            [self Clean];
            return DecryptedChars;
    }
    else
    {//Failure state
        printf("Failed to load decryption key\n");
        NSString * FailureString = @"Failed to load decryption key";
        char * ErrorChars = [self ConvertNSStringToChars:FailureString];
        [self Clean];
        return ErrorChars;
    }
}

- (char *)ConvertNSStringToChars:(NSString *)str {
    const char *strChars = [str UTF8String];
    return (char*)strChars;
}
@end