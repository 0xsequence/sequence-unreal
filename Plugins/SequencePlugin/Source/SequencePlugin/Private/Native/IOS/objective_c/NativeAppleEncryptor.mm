

#import "NativeAppleEncryptor.h"
#import <Foundation/Foundation.h>
#import <Security/Security.h>

static SecKeyAlgorithm algorithm = kSecKeyAlgorithmRSAEncryptionOAEPSHA512AESGCM;
static NSString * TagRef = @"com.Sequence.keys.Main";
static SecKeyRef PrivateKeyRef = NULL;
static SecKeyRef PublicKeyRef = NULL;

@implementation NativeAppleEncryptor

- (BOOL) GenerateKeys
{
    NSData* idTag = [TagRef dataUsingEncoding:NSUTF8StringEncoding];
    NSDictionary* attributes =
    @{ (id)kSecAttrKeyType:               (id)kSecAttrKeyTypeRSA,
       (id)kSecAttrKeySizeInBits:         @2048,
       (id)kSecPrivateKeyAttrs:
       @{ (id)kSecAttrIsPermanent:    @YES,
          (id)kSecAttrApplicationTag: idTag,
       },
    };
    
    CFErrorRef error = NULL;
    PrivateKeyRef = SecKeyCreateRandomKey((__bridge CFDictionaryRef)attributes, &error);
    
    if (PrivateKeyRef)
    {
        PublicKeyRef = SecKeyCopyPublicKey(PrivateKeyRef);
        return true;
    }
    else
    {
        NSLog(@"Error generating keys");
        return false;
    }
}

- (BOOL) LoadKeys 
{
    NSDictionary *query = @ {
        (__bridge id)kSecClass: (__bridge id)kSecClassKey,
        (__bridge id)kSecAttrApplicationTag: TagRef,
        (__bridge id)kSecReturnRef: @YES,
    };
    
    OSStatus status = SecItemCopyMatching((__bridge CFDictionaryRef)query, (CFTypeRef *)&PrivateKeyRef);
    
    if (status == errSecSuccess) 
    {
        PublicKeyRef = SecKeyCopyPublicKey(PrivateKeyRef);
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
    if (PrivateKeyRef) { CFRelease(PrivateKeyRef);}
    if (PublicKeyRef) { CFRelease(PublicKeyRef);}
}

- (char *)Encrypt:(NSString *)str
{
    if ([self LoadKeys])
    {
        CFDataRef plainText = (__bridge CFDataRef)[str dataUsingEncoding:NSUTF8StringEncoding];
        CFErrorRef error = NULL;
        
        CFDataRef cfEncryptedData = SecKeyCreateEncryptedData(PublicKeyRef,algorithm,plainText,&error);

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
    if (str == nil || [str length] == 0) {
        NSString *FailureString = @"Invalid input string";
        return [self ConvertNSStringToChars:FailureString];
    }

    if ([self LoadKeys])
    {
        NSData *DecodedData = [[NSData alloc] initWithBase64EncodedString:str options:0];
        if (DecodedData == nil) {
            NSString *FailureString = @"Failed to decode base64 input";
            return [self ConvertNSStringToChars:FailureString];
        }

        CFDataRef plainText = (__bridge CFDataRef)DecodedData;
        CFErrorRef error = NULL;
            
        CFDataRef cfDecryptedData = SecKeyCreateDecryptedData(PrivateKeyRef, algorithm, plainText, &error);
        
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
            NSString *errorString = err ? err.localizedDescription : @"Unknown decryption error";
            char * ErrorChars = [self ConvertNSStringToChars:errorString];
            [self Clean];
            return ErrorChars;
        }
    }
    else
    {
        NSString * FailureString = @"Failed to load keys";
        return [self ConvertNSStringToChars:FailureString];
    }
}

- (char *)ConvertNSStringToChars:(NSString *)str {
    const char *strChars = [str UTF8String];
    return (char*)strChars;
}
@end
