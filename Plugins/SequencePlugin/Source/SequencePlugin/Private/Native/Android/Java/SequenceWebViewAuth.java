package com.Plugins.SequencePlugin;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.WebResourceRequest;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import androidx.annotation.Nullable;

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
        Bundle args = new Bundle();
        args.putString(WebViewDialogFragment.ARG_REQUEST_URL, requestUrl);
        args.putString(WebViewDialogFragment.ARG_REDIRECT_URL, redirectUrl);

        WebViewDialogFragment dialogFragment = new WebViewDialogFragment();
        dialogFragment.setArguments(args);

        dialogFragment.show(activity.getFragmentManager(), TAG);
    }

    public static class WebViewDialogFragment extends DialogFragment {
        public static final String ARG_REQUEST_URL = "ARG_REQUEST_URL";
        public static final String ARG_REDIRECT_URL = "ARG_REDIRECT_URL";

        private WebView mWebView;

        private String requestUrl = "";
        private String redirectUrl = "";

        private void initArguments() {
            Bundle args = getArguments();

            if (args == null) {
                return;
            }

            requestUrl = args.getString(ARG_REQUEST_URL, "");
            redirectUrl = args.getString(ARG_REDIRECT_URL, "");
        }

        @Override
        public void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);

            initArguments();

            setStyle(DialogFragment.STYLE_NO_TITLE, 0);
        }

        @Nullable
        @Override
        public View onCreateView(
            LayoutInflater inflater, 
            @Nullable ViewGroup container, 
            Bundle savedInstanceState
        ) {
            mWebView = createWebView(
                    getContext(),
                    redirectUrl,
                    redirect -> {
                        GameActivity.sequenceGetInstance().nativeSequenceHandleRedirectUrl(redirect);
                        dismiss();
                    }
            );

            if (savedInstanceState != null) {
                mWebView.restoreState(savedInstanceState);
            } else {
                mWebView.loadUrl(requestUrl);
            }

            return mWebView;
        }

        @Override
        public void onSaveInstanceState(Bundle outState) {
            if (mWebView != null) {
                mWebView.saveState(outState);
            }
            super.onSaveInstanceState(outState);
        }

        @Override
        public void onStart()
        {
            super.onStart();
            Dialog dialog = getDialog();
            if (dialog != null)
            {
                dialog.getWindow().setLayout(
                        ViewGroup.LayoutParams.MATCH_PARENT,
                        ViewGroup.LayoutParams.MATCH_PARENT
                );
            }
        }

        private WebView createWebView(
            Context context,
            String redirectUrl,
            OnRedirectListener onRedirectListener
        ) {
            WebView webView = new WebView(context);

            webView.setVerticalScrollBarEnabled(false);
            webView.setHorizontalScrollBarEnabled(false);
            webView.getSettings().setJavaScriptEnabled(true);

            Client client = new Client(
                    redirectUrl,
                    onRedirectListener
            );
            webView.setWebViewClient(client);

            return webView;
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
