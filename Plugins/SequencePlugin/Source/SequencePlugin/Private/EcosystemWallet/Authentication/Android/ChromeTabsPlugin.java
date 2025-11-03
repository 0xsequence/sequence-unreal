package xyz.sequence;

import android.app.Activity;
import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.net.Uri;

import androidx.browser.customtabs.CustomTabColorSchemeParams;
import androidx.browser.customtabs.CustomTabsClient;
import androidx.browser.customtabs.CustomTabsIntent;

import com.unity3d.player.UnityPlayer;

public final class ChromeTabsPlugin {
    private ChromeTabsPlugin() {}
    
    public static native void OnDeepLinkReceived(String url);
    
    // Handle deep-link intent
    private static void handleIntent(Intent intent) {
        if (intent == null) return;
        Uri data = intent.getData();
        if (data != null) {
            OnDeepLinkReceived(data.toString());
        }
    }
    
    public static void register(Activity activity) {
        // Check launch intent
        handleIntent(activity.getIntent());
    
        activity.getApplication().registerActivityLifecycleCallbacks(
            new android.app.Application.ActivityLifecycleCallbacks() {
                @Override public void onActivityCreated(Activity a, Bundle b) {}
                @Override public void onActivityStarted(Activity a) {}
                @Override public void onActivityResumed(Activity a) {
                    if (a == activity) {
                        handleIntent(a.getIntent());
                    }
                }
                @Override public void onActivityPaused(Activity a) {}
                @Override public void onActivityStopped(Activity a) {}
                @Override public void onActivitySaveInstanceState(Activity a, Bundle outState) {}
                @Override public void onActivityDestroyed(Activity a) {}
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
        final Activity activity = UnityPlayer.currentActivity;
        if (activity == null || url == null) return;

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

    public static boolean isCustomTabsSupported() {
        Activity activity = UnityPlayer.currentActivity;
        if (activity == null) return false;
        String pkg = CustomTabsClient.getPackageName(activity, null);
        return pkg != null;
    }
}