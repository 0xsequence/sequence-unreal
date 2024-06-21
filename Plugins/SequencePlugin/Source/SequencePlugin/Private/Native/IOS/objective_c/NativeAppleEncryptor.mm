// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#import "NativeAppleEncryptor.h"
#import <Foundation/Foundation.h>
#import <Security/Security.h>

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
        NSLog(@"Error generating keys");
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
        return [self GenerateKeys];
    }
    else
    {
        NSLog(@"Keychain error");
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
    if ([self LoadKeys])
    {
        CFDataRef plainText = (__bridge CFDataRef)[str dataUsingEncoding:NSUTF8StringEncoding];
        CFErrorRef error = NULL;
        
        CFDataRef cfEncryptedData = SecKeyCreateEncryptedData(PublicKey,algorithm,plainText,&error);

        if (cfEncryptedData)
        {
            NSData * EncryptedData = (__bridge NSData *)cfEncryptedData;
            NSString * EncryptedDataString = [EncryptedData base64EncodedStringWithOptions:0];
            char * EncryptedChars = [self ConvertNSStringToChars:EncryptedDataString];
            [self Clean];
            return EncryptedChars;
        }
        else
        {
            NSError *err = CFBridgingRelease(error);
            char * ErrorChars = [self ConvertNSStringToChars:err.localizedDescription];
            [self Clean];
            return ErrorChars;
        }
    }
    else
    {
        NSString * FailureString = @"Failed to load keys";
        char * ErrorChars = [self ConvertNSStringToChars:FailureString];
        [self Clean];
        return ErrorChars;
    }
}

- (char *)Decrypt:(NSString *)str
{    
    if ([self LoadKeys])
    {
        NSData *DecodedData = [[NSData alloc] initWithBase64EncodedString:str options:0];
        CFDataRef plainText = (__bridge CFDataRef)DecodedData;
        CFErrorRef error = NULL;
            
        CFDataRef cfDecryptedData = SecKeyCreateDecryptedData(PrivateKey,algorithm,plainText,&error);
        
        if (cfDecryptedData)
        {
            NSData * DecryptedData = (NSData *)CFBridgingRelease(cfDecryptedData);
            NSString * DecryptedDataString = [[NSString alloc] initWithData:DecryptedData encoding:NSUTF8StringEncoding];
            char * DecryptedChars = [self ConvertNSStringToChars:DecryptedDataString];
            [self Clean];
            return DecryptedChars;
        }
        else
        {
            NSError *err = CFBridgingRelease(error);
            char * ErrorChars = [self ConvertNSStringToChars:err.localizedDescription];
            [self Clean];
            return ErrorChars;
        }
    }
    else
    {
        NSString * FailureString = @"Failed to load keys";
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