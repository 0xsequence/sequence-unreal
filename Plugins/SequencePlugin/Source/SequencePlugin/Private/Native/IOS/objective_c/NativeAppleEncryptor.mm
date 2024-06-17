// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#import "NativeAppleEncryptor.h"
#import <Foundation/Foundation.h>
#import <Security/Security.h>
#import <CommonCrypto/CommonCrypto.h>

static SecKeyRef privateKey = NULL;
static SecKeyRef publicKey = NULL;
static NSData * keyRef = NULL:
static NSString * ErrorCapture = @"";
static NSString * IdentifierTag = @"com.Sequence.keys.SymMain";
static SecKeyAlgorithm algorithm = kSecKeyAlgorithmRSAEncryptionOAEPSHA512AESGCM;

@implementation NativeAppleEncryptor

- (BOOL) GenerateKeys 
{
        keyRef = (NSData*)[NSMutableData dataWithLength:kCCKeySizeAES256];
        int result = SecRandomCopyBytes(kSecRandomDefault, keySize, keyData.mutableBytes);
        if (result == errSecSuccess) {
            
            NSDictionary *query = @{
                (__bridge id)kSecClass: (__bridge id)kSecClassKey,
                (__bridge id)kSecAttrKeyType: (__bridge id)kSecAttrKeyTypeAES,
                (__bridge id)kSecAttrKeyClass: (__bridge id)kSecAttrKeyClassSymmetric,
                (__bridge id)kSecAttrApplicationTag: IdentifierTag,
                (__bridge id)kSecValueData: keyRef
            };
        
            OSStatus status = SecItemAdd((__bridge CFDictionaryRef)query, NULL);
            if (status != errSecSuccess) {
                ErrorCapture = @"Error storing symmetric key.";
                NSLog(@"Error storing symmetric key: %d", (int)status);
            }
            return true;
        } else {
            ErrorCapture = @"Error generating symmetric key.";
            NSLog(@"Error generating symmetric key: %d", result);
            return false
        }
}

- (BOOL) LoadKeys 
{
  NSDictionary *query = @{
         (__bridge id)kSecClass: (__bridge id)kSecClassKey,
         (__bridge id)kSecAttrKeyType: (__bridge id)kSecAttrKeyTypeAES,
         (__bridge id)kSecAttrKeyClass: (__bridge id)kSecAttrKeyClassSymmetric,
         (__bridge id)kSecAttrApplicationTag: IdentifierTag,
         (__bridge id)kSecReturnData: @YES
     };
 
     CFTypeRef result = NULL;
     OSStatus status = SecItemCopyMatching((__bridge CFDictionaryRef)query, &result);
     if (status == errSecSuccess) {
        keyRef = (__bridge_transfer NSData*)result;
        return true;
     } else {
        ErrorCapture = @"Error retrieving symmetric key.";
        NSLog(@"Error retrieving symmetric key: %d", (int)status);
        return [self GenerateKeys];
     }
}

- (void) Clean
{
    if (publicKey)  { CFRelease(publicKey);  }
    if (privateKey) { CFRelease(privateKey); }
}

- (char *)Encrypt:(NSString *)str
{
    NSString * TestString = @"Some epic easy string";
    NSLog(@"Input to encrypt: %@", TestString);
    if ([self LoadKeys])
    {
        
/*         if (!SecKeyIsAlgorithmSupported(publicKey, kSecKeyOperationTypeEncrypt, algorithm))
        {
            ErrorCapture = @"Key generated doesn't support set algorithm / operation";
            char * ErrorChars = [self ConvertNSStringToChars:ErrorCapture];
            [self Clean];
            return ErrorChars;
        } */
        
        NSData *dataToEncrypt = [TestString dataUsingEncoding:NSUTF8StringEncoding];
        NSData *symmetricKey = keyRef;
        NSData *EncryptedData = [self encryptData:dataToEncrypt withSymmetricKey:symmetricKey];
        
        NSString * EncryptedDataString = [[NSString alloc] initWithData:EncryptedData encoding:NSUTF8StringEncoding];
        NSLog(@"Encrypted Data: %@", EncryptedDataString);
        char * EncryptedChars = [self ConvertNSStringToChars:EncryptedDataString];
        //[self Clean];
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
            
            if (!SecKeyIsAlgorithmSupported(privateKey, kSecKeyOperationTypeDecrypt, algorithm))
            {
                ErrorCapture = @"Key generated doesn't support set algorithm / operation";
                char * ErrorChars = [self ConvertNSStringToChars:ErrorCapture];
                [self Clean];
                return ErrorChars;
            }
            
            NSData * DecryptedData = (NSData*)CFBridgingRelease(SecKeyCreateDecryptedData(privateKey,algorithm,plainText,&error));
            
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