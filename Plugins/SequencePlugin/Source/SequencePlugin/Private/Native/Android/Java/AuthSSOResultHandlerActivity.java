// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
package com.Plugins.SequencePlugin;

import android.util.Log;
import android.net.Uri;
import android.content.Intent;
import android.app.Application;
import android.content.Context;
import androidx.appcompat.app.AppCompatActivity;

import net.openid.appauth.AuthorizationService;
import net.openid.appauth.AuthorizationServiceConfiguration;
import net.openid.appauth.AuthorizationRequest;
import net.openid.appauth.ResponseTypeValues;
import net.openid.appauth.AuthorizationResponse;
import net.openid.appauth.AuthorizationException;
import net.openid.appauth.TokenResponse;

public class AuthSSOResultHandlerActivity extends AppCompatActivity {
    private static final String TAG = "SequenceGenericSSOSignIn";
    private static final int RC_AUTH = 100;
    private AuthorizationService authService;
        public void GenericSSO(Context context, String AuthEndpoint, String TokenEndpoint, String clientId, String redirectUri)
        {
            AuthorizationServiceConfiguration config = new AuthorizationServiceConfiguration(Uri.parse(AuthEndpoint), Uri.parse(TokenEndpoint));
            //apple may require scope here
            AuthorizationRequest req = new AuthorizationRequest.Builder(config, clientId, ResponseTypeValues.CODE, Uri.parse(redirectUri)).build();
            this.authService = new AuthorizationService(context);
            Intent authIntent = this.authService.getAuthorizationRequestIntent(req);
            startActivityForResult(authIntent, RC_AUTH);
        }
        
        @Override
        protected void onActivityResult(int requestCode, int resultCode, Intent data) {
          if (requestCode == RC_AUTH) {
            AuthorizationResponse resp = AuthorizationResponse.fromIntent(data);
            AuthorizationException ex = AuthorizationException.fromIntent(data);
            
            if (resp != null) {
                //process response
                
                //do token exchange
                this.authService.performTokenRequest(
                    resp.createTokenExchangeRequest(),
                    new AuthorizationService.TokenResponseCallback() {
                      @Override public void onTokenRequestCompleted(
                            TokenResponse resp, AuthorizationException ex) {
                          if (resp != null) {
                            // exchange succeeded
                            //GameActivity.sequenceGetInstance().nativeSequenceHandleSSOIdToken(idTokenCredential.getIdToken());
                            Log.w(TAG, "Token Retrieved");
                          } else {
                          Log.w(TAG, "Failed to retrieve configuration: ", ex);
                          }
                        }
                    });
            }
            else {
            Log.w(TAG, "Failed to retrieve configuration: ", ex);
            }
          } else {
            Log.w(TAG, "Failed improper auth response received");
          }
        }
}