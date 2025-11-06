package com.Plugins.SequencePlugin;

import android.app.Activity;
import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;

import androidx.browser.customtabs.CustomTabColorSchemeParams;
import androidx.browser.customtabs.CustomTabsClient;
import androidx.browser.customtabs.CustomTabsIntent;

import com.epicgames.unreal.GameActivity;

public final class ChromeTabsPlugin {
    private ChromeTabsPlugin() {}

    private static final String TAG = "SequenceDeepLink";    
    private static boolean registered = false;
    
    public static native void OnDeepLinkReceived(String url);
    
    // Handle deep-link intent
    private static void handleIntent(Intent intent) {
        if (intent == null) return;
        Uri data = intent.getData();
        if (data != null) {
            Log.d(TAG, "handleIntent: deep link received -> " + data.toString());
            OnDeepLinkReceived(data.toString());
        }
    }
    
    public static void register(Activity activity) {
        if (registered || activity == null)
            return;
    
        registered = true; 
    
        // Check launch intent
        handleIntent(activity.getIntent());
    
        activity.getApplication().registerActivityLifecycleCallbacks(
            new android.app.Application.ActivityLifecycleCallbacks() {
                @Override
                public void onActivityCreated(Activity a, Bundle b) {
                    Log.d(TAG, "onActivityCreated: " + a.getClass().getSimpleName());
                }

                @Override
                public void onActivityStarted(Activity a) {
                    Log.d(TAG, "onActivityStarted: " + a.getClass().getSimpleName());
                }

                @Override
                public void onActivityResumed(Activity a) {
                    Log.d(TAG, "onActivityResumed: " + a.getClass().getSimpleName());
                    if (a == activity) {
                        Log.d(TAG, "onActivityResumed: Checking for deep link via getIntent()");
                        handleIntent(a.getIntent());
                    }
                }

                @Override
                public void onActivityPaused(Activity a) {
                    Log.d(TAG, "onActivityPaused: " + a.getClass().getSimpleName());
                }

                @Override
                public void onActivityStopped(Activity a) {
                    Log.d(TAG, "onActivityStopped: " + a.getClass().getSimpleName());
                }

                @Override
                public void onActivitySaveInstanceState(Activity a, Bundle outState) {
                    Log.d(TAG, "onActivitySaveInstanceState: " + a.getClass().getSimpleName());
                }

                @Override
                public void onActivityDestroyed(Activity a) {
                    Log.d(TAG, "onActivityDestroyed: " + a.getClass().getSimpleName());
                }
            }
        );
    }
        
    public static void onNewIntent(Intent intent) {
        handleIntent(intent);
    }

    public static void openUrl(final String url) {
        openUrl(url, 0, false, true);
    }

    public static void openUrl(final String url, final int toolbarColor, final boolean enableShare, final boolean showTitle) {
        final Activity activity = GameActivity.Get();
        if (activity == null || url == null) return;
        
        ChromeTabsPlugin.register(activity);

        activity.runOnUiThread(new Runnable() {
            @Override public void run() {
                try {
                    CustomTabsIntent.Builder builder = new CustomTabsIntent.Builder();

                    if (toolbarColor != 0) {
                        CustomTabColorSchemeParams params = new CustomTabColorSchemeParams.Builder()
                                .setToolbarColor(toolbarColor)
                                .build();
                        builder.setDefaultColorSchemeParams(params);
                    }

                    builder.setShareState(enableShare ? CustomTabsIntent.SHARE_STATE_ON : CustomTabsIntent.SHARE_STATE_OFF);
                    builder.setShowTitle(showTitle);

                    CustomTabsIntent customTabsIntent = builder.build();

                    // Prefer a browser that supports Custom Tabs.
                    String pkg = CustomTabsClient.getPackageName(activity, null);
                    if (pkg != null) {
                        customTabsIntent.intent.setPackage(pkg);
                    }

                    Uri uri = Uri.parse(url);
                    customTabsIntent.launchUrl(activity, uri);
                } catch (ActivityNotFoundException e) {
                    // Fallback to a regular VIEW intent if no Custom Tabs capable browser.
                    try {
                        Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
                        activity.startActivity(intent);
                    } catch (Exception ignored) {}
                } catch (Exception ignored) {}
            }
        });
    }
    
    public static void logMessage(final String message) {
        Log.d(TAG, message);
    }
}