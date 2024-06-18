// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#import "NativeAppleEncryptor.h"
#import <Foundation/Foundation.h>
#import <Security/Security.h>

static NSString * ErrorCapture = @"";
static SecKeyAlgorithm algorithm = kSecKeyAlgorithmRSAEncryptionOAEPSHA512AESGCM;
static NSString * Tag = @"com.Sequence.keys.Main";
static SecKeyRef PrivateKey = NULL;
static SecKeyRef PublicKey = NULL;

@implementation NativeAppleEncryptor

- (bool) GenerateKeys
{
    NSDictionary *keyAttributes = @{
        (__bridge id)kSecAttrKeyType: (__bridge id)kSecAttrKeyTypeRSA,
        (__bridge id)kSecAttrKeySizeInBits: @2048
    };
    
    OSStatus status = SecKeyGeneratePair((__bridge CFDictionaryRef)keyAttributes, &PublicKey, &PrivateKey);

    if (status == errSecSuccess) 
    {            
        NSDictionary *privateKeyAttributes = @{
            (__bridge id)kSecValueRef: (__bridge id)PrivateKey,
            (__bridge id)kSecAttrLabel: Tag,
            (__bridge id)kSecAttrKeyClass: (__bridge id)kSecAttrKeyClassPrivate,
        };
            
        status = SecItemAdd((__bridge CFDictionaryRef)privateKeyAttributes, NULL);
        if (status != errSecSuccess)
        {
            NSLog(@"Error storing private key in keychain");
        }
        return (status == errSecSuccess);
    }
    NSLog(@"Error generating RSA key pair");
    return false;
}

- (bool) LoadKeys 
{
    NSDictionary *query = @ {
        (__bridge id)kSecClass: (__bridge id)kSecClassKey,
        (__bridge id)kSecAttrApplicationTag: Tag,
        (__bridge id)kSecReturnRef: @YES,
    };
    
    OSStatus status = SecItemCopyMatching((__bridge CFDictionaryRef)query, (CFTypeRef *)&PrivateKey);
    
    if (status == errSecSuccess) 
    {
        PublicKey = SecKeyCopyPublicKey(PrivateKey);
        return true;
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
        return false;
    }
}

- (void) Clean:
{
    if (PrivateKey) { CFRelease(PrivateKey);}
    if (PublicKey { CFRelease(PublicKey);}
}

- (char *)Encrypt:(NSString *)str
{
    NSString * TestString = @"Some epic easy string";
    NSLog(@"Input to encrypt: %@", TestString);
    
    if ([self LoadKeys])
    {
        CFDataRef plainText = (__bridge CFDataRef)[TestString dataUsingEncoding:NSUTF8StringEncoding];
        CFErrorRef error = NULL;
        
        if (!SecKeyIsAlgorithmSupported(PublicKey, kSecKeyOperationTypeEncrypt, algorithm))
        {
            ErrorCapture = @"Key generated doesn't support set algorithm / operation";
            char * ErrorChars = [self ConvertNSStringToChars:ErrorCapture];
            [self Clean];
            return ErrorChars;
        }

        NSData * EncryptedData = (NSData*)CFBridgingRelease(SecKeyCreateEncryptedData(PublicKey,algorithm,plainText,&error));
        
        if (error)
        {
            NSError *err = CFBridgingRelease(error);
            ErrorCapture = err.localizedDescription;
            NSLog(@"Captured Error: %@", err);
            char * ErrorChars = [self ConvertNSStringToChars:ErrorCapture];
            [self Clean];
            return ErrorChars;
        }
        
        NSString * EncryptedDataString = [[NSString alloc] initWithData:EncryptedData encoding:NSUTF8StringEncoding];
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
            
        if (!SecKeyIsAlgorithmSupported(PrivateKey, kSecKeyOperationTypeDecrypt, algorithm))
        {
            ErrorCapture = @"Key generated doesn't support set algorithm / operation";
            char * ErrorChars = [self ConvertNSStringToChars:ErrorCapture];
            [self Clean];
            return ErrorChars;
        }
            
        NSData * DecryptedData = (NSData*)CFBridgingRelease(SecKeyCreateDecryptedData(PrivateKey,algorithm,plainText,&error));
            
        if (error)
        {
            NSError *err = CFBridgingRelease(error);
            ErrorCapture = err.localizedDescription;
            NSLog(@"Captured Error: %@", err);
            char * ErrorChars = [self ConvertNSStringToChars:ErrorCapture];
            [self Clean];
            return ErrorChars;
        }
            
        NSString * DecryptedDataString = [[NSString alloc] initWithData:DecryptedData encoding:NSUTF8StringEncoding];
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