package com.Plugins.SequencePlugin;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.graphics.Rect;
import android.os.Bundle;
import android.util.Log;
import android.view.ViewGroup;
import android.view.Window;
import android.webkit.WebResourceRequest;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import com.epicgames.unreal.GameActivity;

public class SequenceWebViewAuth {
    private static final String TAG = "SequenceWebViewAuth";

    public static void requestAuth(
        Activity activity,
        String requestUrl,
        String redirectUrl
    ) {
        activity.runOnUiThread(() -> {
            _requestAuth(activity, requestUrl, redirectUrl);
        });
    }

    private static void _requestAuth(
        Activity activity,
        String requestUrl,
        String redirectUrl        
    ) {
        Dialog dialog = new Dialog(activity);

        WebView webView = new WebView(activity);

        webView.setVerticalScrollBarEnabled(false);
        webView.setHorizontalScrollBarEnabled(false);
        webView.getSettings().setJavaScriptEnabled(true);

        Client client = new Client(
                redirectUrl,
                redirect -> {
                    GameActivity.sequenceGetInstance().nativeSequenceHandleRedirectUrl(redirect);
                    dialog.dismiss();
                }
        );
        webView.setWebViewClient(client);

        webView.loadUrl(requestUrl);

        dialog.setContentView(webView);

        dialog.show();
        initDialogLayout(dialog, activity);
    }

    private static void initDialogLayout(Dialog dialog, Activity activity) {
        Rect displayRectangle = new Rect();
        Window window = activity.getWindow();
        window.getDecorView().getWindowVisibleDisplayFrame(displayRectangle);

        Window dialogWindow = dialog.getWindow();
        if (dialogWindow != null) {
            dialogWindow.setLayout(
                (int) (displayRectangle.width() * 0.9f),
                (int) (displayRectangle.height() * 0.9f)
            );
        }
    }

    private interface OnRedirectListener {
        void onRedirect(String redirect);
    }

    private static class Client extends WebViewClient {
        private String redirectUrl;
        private OnRedirectListener redirectListener;

        public Client(String redirectUrl, OnRedirectListener redirectListener) {
            this.redirectUrl = redirectUrl;
            this.redirectListener = redirectListener;
        }

        @Override
        public boolean shouldOverrideUrlLoading(WebView view, WebResourceRequest request) {
            if (request.getUrl().toString().startsWith(redirectUrl)) {
                if (redirectListener != null) {
                    redirectListener.onRedirect(request.getUrl().toString());
                    return true;
                }
            }
            return super.shouldOverrideUrlLoading(view, request);
        }
    }
}