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
    NSData* idTag = [Tag dataUsingEncoding:NSUTF8StringEncoding];
    NSDictionary* attributes =
    @{ (id)kSecAttrKeyType:               (id)kSecAttrKeyTypeRSA,
       (id)kSecAttrKeySizeInBits:         @2048,
       (id)kSecPrivateKeyAttrs:
       @{ (id)kSecAttrIsPermanent:    @YES,
          (id)kSecAttrApplicationTag: idTag,
       },
    };
    
    CFErrorRef error = NULL;
    PrivateKey = SecKeyCreateRandomKey((__bridge CFDictionaryRef)attributes, &error);
    
    if (PrivateKey)
    {
        PublicKey = SecKeyCopyPublicKey(PrivateKey);
        return true;
    }
    else
    {
        NSLog(@"Error generating RSA key pair");
        return false;
    }
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

- (void) Clean
{
    if (PrivateKey) { CFRelease(PrivateKey);}
    if (PublicKey) { CFRelease(PublicKey);}
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
        
        CFDataRef cfEncryptedData = SecKeyCreateEncryptedData(PublicKey,algorithm,plainText,&error);
        CFIndex ByteLength = CFDataGetLength(cfEncryptedData);
        NSLog(@"Byte Length: %lu",ByteLength);
        if (cfEncryptedData)
        {
            NSData * EncryptedData = (__bridge NSData *)cfEncryptedData;
            NSString * EncryptedDataString = [[NSString alloc] initWithData:EncryptedData encoding:NSUTF8StringEncoding];
            NSLog(@"Encrypted Data: %@", EncryptedDataString);
            char * EncryptedChars = [self ConvertNSStringToChars:EncryptedDataString];
            [self Clean];
            return EncryptedChars;
        }
        else
        {
            NSError *err = CFBridgingRelease(error);
            ErrorCapture = err.localizedDescription;
            NSLog(@"Captured Error: %@", err);
            char * ErrorChars = [self ConvertNSStringToChars:ErrorCapture];
            [self Clean];
            return ErrorChars;
        }
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
            
        CFDataRef cfDecryptedData = SecKeyCreateDecryptedData(PrivateKey,algorithm,plainText,&error);
        
        if (cfDecryptedData)
        {
            NSData * DecryptedData = (NSData *)CFBridgingRelease(cfDecryptedData);
            NSString * DecryptedDataString = [[NSString alloc] initWithData:DecryptedData encoding:NSUTF8StringEncoding];
            NSLog(@"Decrypted Data: %@", DecryptedDataString);
            char * DecryptedChars = [self ConvertNSStringToChars:DecryptedDataString];
            [self Clean];
            return DecryptedChars;
        }
        else
        {
            NSError *err = CFBridgingRelease(error);
            ErrorCapture = err.localizedDescription;
            NSLog(@"Captured Error: %@", err);
            char * ErrorChars = [self ConvertNSStringToChars:ErrorCapture];
            [self Clean];
            return ErrorChars;
        }
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