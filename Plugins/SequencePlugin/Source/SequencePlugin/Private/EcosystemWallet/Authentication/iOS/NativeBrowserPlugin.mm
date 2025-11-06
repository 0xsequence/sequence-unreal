#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <SafariServices/SafariServices.h>
#import <objc/runtime.h>
#import <objc/message.h>

static SFSafariViewController *safariVC = nil;

extern "C" {
    void OpenWalletApp(const char *urlCString)
    {
        NSString *urlString = [NSString stringWithUTF8String:urlCString];
        NSURL *url = [NSURL URLWithString:urlString];
        if (!url) return;

        UIViewController *rootVC = [UIApplication sharedApplication].keyWindow.rootViewController;
        safariVC = [[SFSafariViewController alloc] initWithURL:url];
        
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
