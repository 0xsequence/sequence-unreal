// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#import "NativeAppleEncryptor.h"
#import <Foundation/Foundation.h>
#import <Security/Security.h>

static NSString * ErrorCapture = @"";
static SecKeyAlgorithm algorithm = kSecKeyAlgorithmRSAEncryptionOAEPSHA512AESGCM;

@implementation NativeAppleEncryptor

- (SecKeyRef) GenerateKeys
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
    SecKeyRef PrivateKey = SecKeyCreateRandomKey((__bridge CFDictionaryRef)attributes,&error);
    
    if (!privateKey) {
        NSError *err = CFBridgingRelease(error);
        ErrorCapture = err.localizedDescription;
        printf("Failed to generate private key\n");
        return NULL;
    }
    return PrivateKey;
}

- (SecKeyRef) LoadKeys 
{
    NSDictionary *query = @ {
        (__bridge id)kSecClass: (__bridge id)kSecClassKey,
        (__bridge id)kSecAttrApplicationTag: @"com.Sequence.keys.Main",
        (__bridge id)kSecReturnRef: @YES,
    };
    
    SecKeyRef PrivateKey = NULL;
    OSStatus status = SecItemCopyMatching((__bridge CFDictionaryRef)query, (CFTypeRef *)&PrivateKey);
    
    if (status == errSecSuccess) 
    {
        return PrivateKey;
    }
    else if (status == errSecItemNotFound)
    {
        ErrorCapture = @"Keys not found generating fresh keys";
        printf("Keys not found generating fresh keys\n");
        return [self GenerateKeys];
    }
    else
    {
        ErrorCapture = @"KeyChain Error";
        printf("KeyChain Error\n");
        return NULL;
    }
}

- (void) Clean:(SecKeyRef)Key
{
    if (Key)
    {
        CFRelease(Key);
    }
}

- (char *)Encrypt:(NSString *)str
{
    NSString * TestString = @"Some epic easy string";
    NSLog(@"Input to encrypt: %@", TestString);
    SecKeyRef PrivateKey = [self LoadKeys];
    if (PrivateKey)
    {
        CFDataRef plainText = (__bridge CFDataRef)[TestString dataUsingEncoding:NSUTF8StringEncoding];
        CFErrorRef error = NULL;
        SecKeyRef PublicKey = SecKeyCopyPublicKey(PrivateKey);
        
        if (!SecKeyIsAlgorithmSupported(PublicKey, kSecKeyOperationTypeEncrypt, algorithm))
        {
            ErrorCapture = @"Key generated doesn't support set algorithm / operation";
            char * ErrorChars = [self ConvertNSStringToChars:ErrorCapture];
            [self Clean:PrivateKey];
            [self Clean:PublicKey];
            return ErrorChars;
        }

        NSData * EncryptedData = (NSData*)CFBridgingRelease(SecKeyCreateEncryptedData(publicKey,algorithm,plainText,&error));
        
        if (error)
        {
            NSError *err = CFBridgingRelease(error);
            ErrorCapture = err.localizedDescription;
            NSLog(@"Captured Error: %@", err);
            char * ErrorChars = [self ConvertNSStringToChars:ErrorCapture];
            [self Clean:PrivateKey];
            [self Clean:PublicKey];
            return ErrorChars;
        }
        
        NSString * EncryptedDataString = [[NSString alloc] initWithData:EncryptedData encoding:NSUTF8StringEncoding];
        NSLog(@"Encrypted Data: %@", EncryptedDataString);
        char * EncryptedChars = [self ConvertNSStringToChars:EncryptedDataString];
        [self Clean:PrivateKey];
        [self Clean:PublicKey];
        return EncryptedChars;
    }
    else
    {//Failure state
        printf("Failed to load encryption key\n");
        char * ErrorChars = [self ConvertNSStringToChars:ErrorCapture];
        [self Clean:PrivateKey];
        return ErrorChars;
    }
}

- (char *)Decrypt:(NSString *)str
{
    NSLog(@"Input to decrypt: %@", str);
    
    SecKeyRef PrivateKey = [self LoadKeys];
    
    if (PrivateKey)
    {
            CFDataRef plainText = (__bridge CFDataRef)[str dataUsingEncoding:NSUTF8StringEncoding];
            CFErrorRef error = NULL;
            
            if (!SecKeyIsAlgorithmSupported(PrivateKey, kSecKeyOperationTypeDecrypt, algorithm))
            {
                ErrorCapture = @"Key generated doesn't support set algorithm / operation";
                char * ErrorChars = [self ConvertNSStringToChars:ErrorCapture];
                [self Clean:PrivateKey];
                return ErrorChars;
            }
            
            NSData * DecryptedData = (NSData*)CFBridgingRelease(SecKeyCreateDecryptedData(PrivateKey,algorithm,plainText,&error));
            
            if (error)
            {
                NSError *err = CFBridgingRelease(error);
                ErrorCapture = err.localizedDescription;
                NSLog(@"Captured Error: %@", err);
                char * ErrorChars = [self ConvertNSStringToChars:ErrorCapture];
                [self Clean:PrivateKey];
                return ErrorChars;
            }
            
            NSString * DecryptedDataString = [[NSString alloc] initWithData:DecryptedData encoding:NSUTF8StringEncoding];
            NSLog(@"Decrypted Data: %@", DecryptedDataString);
            char * DecryptedChars = [self ConvertNSStringToChars:DecryptedDataString];
            [self Clean:PrivateKey];
            return DecryptedChars;
    }
    else
    {//Failure state
        printf("Failed to load decryption key\n");
        NSString * FailureString = @"Failed to load decryption key";
        char * ErrorChars = [self ConvertNSStringToChars:FailureString];
        [self Clean:PrivateKey];
        return ErrorChars;
    }
}

- (char *)ConvertNSStringToChars:(NSString *)str {
    const char *strChars = [str UTF8String];
    return (char*)strChars;
}
@end