package com.Plugins.SequencePlugin;


import android.content.Context;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.credentials.Credential;
import androidx.credentials.CredentialManager;
import androidx.credentials.CredentialManagerCallback;
import androidx.credentials.CustomCredential;
import androidx.credentials.GetCredentialRequest;
import androidx.credentials.GetCredentialResponse;
import androidx.credentials.exceptions.GetCredentialException;

import com.google.android.libraries.identity.googleid.GetGoogleIdOption;
import com.google.android.libraries.identity.googleid.GoogleIdTokenCredential;

import java.util.concurrent.Executors;

import com.epicgames.unreal.GameActivity;

public class SequenceGoogleSignInHelper {
    private static final String TAG = "SequenceGoogleSignIn";

    public static void signIn(
        Context context,
        String clientId
    ) {
        getCredentialAsync(context, clientId, true);
    }

    private static getCredentialAsync(
        Context context,
        String clientId,
        boolean filterByAuthorizedAccounts
    ) {
        GetGoogleIdOption googleIdOption = new GetGoogleIdOption.Builder()
                .setFilterByAuthorizedAccounts(filterByAuthorizedAccounts)
                .setServerClientId(clientId)
                // TODO: include a nonce value here
                .build();

        GetCredentialRequest request = new GetCredentialRequest.Builder()
                .addCredentialOption(googleIdOption)
                .build();

        CredentialManager credentialManager = CredentialManager.create(context);
        credentialManager.getCredentialAsync(
                context,
                request,
                null,
                Executors.newSingleThreadExecutor(),
                new CredentialManagerCallback<GetCredentialResponse, GetCredentialException>() {
                    @Override
                    public void onResult(GetCredentialResponse getCredentialResponse) {
                        handleGetCredentialResponse(getCredentialResponse);
                    }

                    @Override
                    public void onError(@NonNull GetCredentialException e) {
                        Log.e(TAG, "Error getting credential", e);
                    }
                }
        );
    }

    private static void handleGetCredentialResponse(GetCredentialResponse getCredentialResponse) {
        Credential credential = getCredentialResponse.getCredential();
        if (credential instanceof CustomCredential &&
                GoogleIdTokenCredential.TYPE_GOOGLE_ID_TOKEN_CREDENTIAL.equals(credential.getType())
        ) {
            try {
                GoogleIdTokenCredential idTokenCredential = GoogleIdTokenCredential.createFrom(credential.getData());
                GameActivity.sequenceGetInstance().nativeSequenceHandleGoogleIdToken(idTokenCredential.getIdToken());
            } catch (Exception e) {
                Log.e(TAG, "Failed to parse Google ID token response", e);
            }
        } else {
            Log.e(TAG, "Unexpected credential type");
        }
    }    
}