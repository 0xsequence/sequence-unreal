#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <SafariServices/SafariServices.h>
#import <objc/runtime.h>
#import <objc/message.h>
#import "IOSAppDelegate.h"
#include "IOS/IOSPlatformMisc.h"

static SFSafariViewController *safariVC = nil;
static id<SFSafariViewControllerDelegate> safariDelegate = nil;

static NSString* EncodeBase64(NSString* input)
{
    NSData* data = [input dataUsingEncoding:NSUTF8StringEncoding];
    return [data base64EncodedStringWithOptions:0];
}

@interface SafariCloseDelegate : NSObject <SFSafariViewControllerDelegate>
@end

@implementation SafariCloseDelegate

- (void)safariViewControllerDidFinish:(SFSafariViewController *)controller
{
    NSLog(@"[SafariPlugin] User closed SafariViewController");
    
    NSString* errorMessage = @"User cancelled the wallet request.";
    NSString* base64 = EncodeBase64(errorMessage);

    NSString* urlString = [NSString stringWithFormat:@"myapp://safari_closed?error=%@", base64];
    NSURL* url = [NSURL URLWithString:urlString];

    FIOSCoreDelegates::OnOpenURL.Broadcast(
        [UIApplication sharedApplication],
        url,
        @"SafariViewController",
        nil
    );

    safariVC = nil;
}

@end

extern "C" {
    void OpenExternalBrowser(const char* urlCString)
    {
        if (!urlCString) return;

        NSString* urlString = [NSString stringWithUTF8String:urlCString];
        NSURL* url = [NSURL URLWithString:urlString];
        if (!url) return;

        dispatch_async(dispatch_get_main_queue(), ^{
            UIApplication* app = [UIApplication sharedApplication];
            if ([app canOpenURL:url])
            {
                if (@available(iOS 10.0, *)) {
                    [app openURL:url options:@{} completionHandler:nil];
                } else {
                    [app openURL:url];
                }
            }
        });
    }
}

extern "C" {
    void OpenWalletApp(const char *urlCString)
    {
        NSString *urlString = [NSString stringWithUTF8String:urlCString];
        NSURL *url = [NSURL URLWithString:urlString];
        if (!url) return;

        UIViewController *rootVC = [UIApplication sharedApplication].keyWindow.rootViewController;
        safariVC = [[SFSafariViewController alloc] initWithURL:url];
        
        safariDelegate = [SafariCloseDelegate new];
        safariVC.delegate = safariDelegate;
        
        if (@available(iOS 11.0, *)) {
            safariVC.dismissButtonStyle = SFSafariViewControllerDismissButtonStyleCancel;
        }
        
        if (@available(iOS 13.0, *)) {
            safariVC.modalInPresentation = YES;
        }

        NSLog(@"[SafariPlugin] Presenting SFSafariViewController with URL: %@", urlString);

        dispatch_async(dispatch_get_main_queue(), ^{
            [rootVC presentViewController:safariVC animated:YES completion:nil];
        });
    }
}

extern "C" {
    void CloseWalletApp()
    {
        if (safariVC)
        {
            dispatch_async(dispatch_get_main_queue(), ^{
                [safariVC dismissViewControllerAnimated:YES completion:nil];
                safariVC = nil;
            });
        }
    }
}
