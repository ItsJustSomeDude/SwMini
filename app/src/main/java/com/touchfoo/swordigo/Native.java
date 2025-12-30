package com.touchfoo.swordigo;

import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.AssetManager;
import android.net.Uri;

import net.itsjustsomedude.swrdg.LNIString;
import net.itsjustsomedude.swrdg.MainActivity;
import net.itsjustsomedude.swrdg.MiniOverlay;
import net.itsjustsomedude.swrdg.ModProperties;

import java.lang.ref.WeakReference;
import java.util.Objects;

/**
 * We have to suppress this because they are libswordigo functions which java doesn't know about.
 *
 * @noinspection JavaJniMissingFunction unused
 */
public class Native {
	public static WeakReference<MainActivity> mainActivityRef;

	public static native void applicationDidBecomeActive();

	public static native void applicationDidBecomeInactive();

	public static native void applicationDidEnterBackground();

	public static native void applicationDidEnterForeground();

	public static MainActivity getActivity() {
		if (mainActivityRef == null) return null;

		return mainActivityRef.get();
	}

	public static boolean checkPromotion(String var0) {
		Debug.Log("Native.checkPromotion: " + var0);
		return false;
	}

	public static void consumePurchasesForTesting() {
		MainActivity mainActivity = getActivity();
		if (mainActivity == null) return;

		mainActivity.runOnUiThread(() -> {
//                Native.mainActivity.storeController.consumePurchasesForTesting();
//                Native.mainActivity.gameServices.adsHelper.enableTestAds();
			mainActivity.persistentState.setDelayMillisToReviewFlow(0L);
			mainActivity.persistentState.setTotalForegroundMillisForReviewFlow(7200000L);
		});
	}

	public static void copyToClipboard(String var0, String var1) {
		MainActivity mainActivity = getActivity();
		if (mainActivity == null) return;

		mainActivity.runOnUiThread(() ->
			((ClipboardManager) mainActivity
				.getSystemService(Context.CLIPBOARD_SERVICE))
				.setPrimaryClip(ClipData.newPlainText(var0, var1))
		);
	}

	public static native void debugFunction();

	public static void deleteSnapshot(String var0) {
//        mainActivity.runOnUiThread(new Runnable(var0) {
//            final String val$name;
//
//            {
//                this.val$name = var1;
//            }
//
//            public void run() {
//                StringBuilder var1 = new StringBuilder("Sync: deleteSnapshot called from native: ");
//                var1.append(this.val$name);
//                Debug.Log(var1.toString());
//                Native.mainActivity.gameServices.deleteSnapshot(this.val$name);
//            }
//        });
	}

	public static native void drawApplication();

	public static void enteredAge(int var0) {
		saveIntInSP("knownAge", var0);
		Debug.Log("Saved age: " + var0);
//        if (isAgeKnown() && !isAgeOfConsent()) {
//            mainActivity.runOnUiThread(() -> {
//                Debug.Log("Not age of consent");
////                    Native.mainActivity.gameServices.adsHelper.start();
//            });
//        }

	}

	public static native void finishedRestoringPurchases();

	public static String getAnalyticsId() {
//        String var1 = mainActivity.analytics.getFormattedAppInstanceId();
//        String var0 = var1;
//        if (var1 == null) {
//            var0 = "";
//        }
//
//        return var0;
		return "";
	}

	public static boolean getBooleanFromSP(String var0) {
		MainActivity mainActivity = getActivity();
		if (mainActivity == null) return false;

		return mainActivity.getApplicationContext().getSharedPreferences("SwordigoPreferences", 0).getBoolean(var0, false);
	}

	public static int getIntFromSP(String var0) {
		MainActivity mainActivity = getActivity();
		if (mainActivity == null) return 0;

		return mainActivity.getApplicationContext().getSharedPreferences("SwordigoPreferences", 0).getInt(var0, 0);
	}

	public static int getInterstitialAdInterval(String var0, int var1) {
//        var1 = (int)RemoteConfig.getInstance().getAdIntervalSeconds(var0, (long)var1);
//        StringBuilder var2 = new StringBuilder("Native.getInterstitialAdInterval: ");
//        var2.append(var0);
//        var2.append(" = ");
//        var2.append(var1);
//        Debug.Log(var2.toString());
//        return var1;
		return 1000000000;
	}

	public static int getPlatformConsentState() {
//        mainActivity.runOnUiThread(new Runnable() {
//            public void run() {
//                Native.mainActivity.gameServices.adsHelper.startConsentInfoRequestIfNecessary();
//            }
//        });
//        int var1 = null.$SwitchMap$com$touchfoo$swordigo$AdsHelper$ConsentStatus[mainActivity.gameServices.adsHelper.getCurrentConsentStatus().ordinal()];
//        byte var0 = 1;
//        if (var1 != 1) {
//            var0 = 2;
//            if (var1 != 2) {
//                return 0;
//            }
//        }
//
//        return var0;
		return 0;
	}

	public static String getStoreName() {
		//return mainActivity.storeController != null ? mainActivity.storeController.storeName() : "";
		return "";
	}

	public static native void googleSignInCompleted(boolean var0);

	public static native void handleApplicationLaunch();

	public static native void handleApplicationQuit();

	public static native void handleBackButtonPress();

	public static native void handleMenuButtonPress();

	public static native void handleTouchEvent(int var0, int var1, double var2, float var4, float var5, float var6, float var7, int var8);

	public static boolean hasPrivacyConsent() {
		return getBooleanFromSP("privacyConsent");
	}

	public static void initiateGoogleSignIn() {
		LNIString.execute(ModProperties.googleAction);
//        mainActivity.runOnUiThread(new Runnable() {
//            public void run() {
//                Debug.Log("initiateGoogleSignIn called from native");
//                if (Native.mainActivity.gamesSignIn != null) {
//                    Native.mainActivity.gamesSignIn.beginInteractiveSignIn();
//                }
//
//            }
//        });
	}

	public static native void interstitialAdVisibilityChanged(boolean var0);

	public static boolean isAgeKnown() {
		return knownAge() != 0;
	}

	public static boolean isAgeOfConsent() {
		return knownAge() >= 16;
	}

	public static boolean isExplicitPrivacyConsent() {
		return getBooleanFromSP("explicitConsent");
	}

	public static boolean isGoogleGameServicesAvailable() {
		return ModProperties.googleVisible;
	}

	public static int knownAge() {
		return getIntFromSP("knownAge");
	}

	public static void loadInterstitialAd() {
//        mainActivity.runOnUiThread(new Runnable() {
//            public void run() {
//                Debug.Log("Sync: loadInterstitialAd() called from native");
//                if (Native.mainActivity.gameServices != null && Native.mainActivity.gameServices.adsHelper != null) {
//                    Native.mainActivity.gameServices.adsHelper.loadInterstitialIfNecessary();
//                }
//
//            }
//        });
	}

	public static void loadSnapshot(String var0, double var1) {
//        mainActivity.runOnUiThread(new Runnable(var0, var1) {
//            final double val$delay;
//            final String val$name;
//
//            {
//                this.val$name = var1;
//                this.val$delay = var2;
//            }
//
//            public void run() {
//                StringBuilder var1 = new StringBuilder("Sync: loadSnapshot called from native: ");
//                var1.append(this.val$name);
//                var1.append(", ");
//                var1.append(this.val$delay);
//                Debug.Log(var1.toString());
//                (new Handler()).postDelayed(new Runnable(this) {
//                    final <undefinedtype> this$0;
//
//                    {
//                        this.this$0 = var1;
//                    }
//
//                    public void run() {
//                        if (this.this$0.val$name != null && this.this$0.val$name.length() != 0) {
//                            Native.mainActivity.gameServices.loadSnapshot(this.this$0.val$name);
//                        } else {
//                            Native.mainActivity.gameServices.loadAllSnapshots();
//                        }
//
//                    }
//                }, (long)((int)(this.val$delay * 1000.0)));
//            }
//        });
	}

	public static void openURL(String url) {
		Debug.Log("Open URL: " + url);

		if (Objects.equals(url, "http://www.twitter.com/touch_foo")) {
			LNIString.execute(ModProperties.twitterLink);
		} else if (Objects.equals(url, "http://www.facebook.com/144881932264830")) {
			LNIString.execute(ModProperties.facebookLink);
		} else if (Objects.equals(url, "http://privacy.touchfoo.com/")) {
//            LuaNativeInterface.processCommand(ModProperties.privacyLink);
			MainActivity a = mainActivityRef.get();
			if (a == null) return;
			a.runOnUiThread(MiniOverlay::show);
		} else {
			// TODO: Fix infinite loop when trying to load a real touchfoo link.
			Intent var1 = new Intent("android.intent.action.VIEW", Uri.parse(url));

			MainActivity mainActivity = getActivity();
			if (mainActivity == null) return;

			mainActivity.startActivity(var1);
		}
	}

	public static void prepareReviewFlow() {
	}

	public static void processStoreQueue() {
//        mainActivity.runOnUiThread(new Runnable() {
//            public void run() {
//                Native.mainActivity.storeController.processQueue();
//            }
//        });
	}

	public static native void productPurchaseFailed(String var0, String var1);

	public static native void productPurchased(String var0);

	public static void purchaseStoreProduct(String var0) {
//        mainActivity.runOnUiThread(new Runnable(var0) {
//            final String val$identifier;
//
//            {
//                this.val$identifier = var1;
//            }
//
//            public void run() {
//                Native.mainActivity.storeController.purchaseProduct(Native.mainActivity, this.val$identifier);
//            }
//        });
	}

	public static void queueStoreProductFetch(String var0) {
		Debug.Log("Store Product Fetch called!");
		debugFunction();

//        mainActivity.runOnUiThread(new Runnable(var0) {
//            final String val$identifier;
//
//            {
//                this.val$identifier = var1;
//            }
//
//            public void run() {
//                Native.mainActivity.storeController.queueProductFetch(this.val$identifier);
//            }
//        });
	}

	public static void quitApplication() {
		MainActivity mainActivity = getActivity();
		if (mainActivity == null) return;

		mainActivity.runOnUiThread(() -> {
			Debug.Log("quitApplication called from native");
			mainActivity.moveTaskToBack(true);
		});
	}

	public static void receivedPrivacyConsent(boolean var0) {
		Debug.Log("Received privacy consent");
		saveBooleanInSP("privacyConsent", true);
		if (var0) {
			saveBooleanInSP("explicitConsent", true);
		}

	}

	public static native void reloadContext();

	public static void reportAchievementProgress(String var0, int var1, boolean var2) {
		Debug.Log("Report Achievements called!");

//        mainActivity.runOnUiThread(new Runnable(var0, var1, var2) {
//            final String val$identifier;
//            final boolean val$isIncremental;
//            final int val$stepsCompleted;
//
//            {
//                this.val$identifier = var1;
//                this.val$stepsCompleted = var2;
//                this.val$isIncremental = var3;
//            }
//
//            public void run() {
//                StringBuilder var1 = new StringBuilder("reportAchievementProgress(");
//                var1.append(this.val$identifier);
//                var1.append(", ");
//                var1.append(this.val$stepsCompleted);
//                var1.append(", ");
//                var1.append(this.val$isIncremental);
//                var1.append(") called from native");
//                Debug.Log(var1.toString());
//                Native.mainActivity.gameServices.reportAchievementProgress(this.val$identifier, this.val$stepsCompleted, this.val$isIncremental);
//            }
//        });
	}

	public static native void reviewFlowCompleted();

	public static void saveBooleanInSP(String var0, boolean var1) {
		MainActivity mainActivity = getActivity();
		if (mainActivity == null) return;

		SharedPreferences.Editor var2 = mainActivity.getApplicationContext().getSharedPreferences("SwordigoPreferences", 0).edit();
		var2.putBoolean(var0, var1);
		var2.apply();
	}

	public static void saveIntInSP(String var0, int var1) {
		MainActivity mainActivity = getActivity();
		if (mainActivity == null) return;

		SharedPreferences.Editor var2 = mainActivity.getApplicationContext().getSharedPreferences("SwordigoPreferences", 0).edit();
		var2.putInt(var0, var1);
		var2.apply();
	}

	public static void saveSnapshot(String var0, byte[] var1, String var2, long var3, long var5) {
		Debug.Log("Save Snapshot called!");
//        mainActivity.runOnUiThread(new Runnable(var0, var1, var2, var3, var5) {
//            final byte[] val$data;
//            final String val$description;
//            final String val$name;
//            final long val$progressValue;
//            final long val$timePlayedMillis;
//
//            {
//                this.val$name = var1;
//                this.val$data = var2;
//                this.val$description = var3;
//                this.val$timePlayedMillis = var4;
//                this.val$progressValue = var6;
//            }
//
//            public void run() {
//                StringBuilder var1 = new StringBuilder("Sync: saveSnapshot called from native: ");
//                var1.append(this.val$name);
//                var1.append(", ");
//                var1.append(this.val$data.length);
//                var1.append(", ");
//                var1.append(this.val$description);
//                var1.append(", ");
//                var1.append(this.val$timePlayedMillis);
//                var1.append(", ");
//                var1.append(this.val$progressValue);
//                Debug.Log(var1.toString());
//                Native.mainActivity.gameServices.saveSnapshot(this.val$name, this.val$data, this.val$description, this.val$timePlayedMillis, this.val$progressValue);
//            }
//        });
	}

	public static native void setApplicationViewSize(int var0, int var1, boolean var2);

	public static native void setApplicationViewSize(int var0, int var1, boolean var2, int var3, int var4);
	// 1.4.10

	public static native void setAssetManager(AssetManager var0);

	public static native void setCacheDir(String var0);

	public static native void setFilesDir(String var0);

	public static native void setupApplication();

	public static native void setupNativeInterface();

	public static void showAchievements() {
		Debug.Log("Achievements called!");
//        mainActivity.runOnUiThread(new Runnable() {
//            public void run() {
//                Debug.Log("showAchievements called from native");
//                Native.mainActivity.gameServices.showAchievements();
//            }
//        });
	}

	public static void showAnalyticsIdPopup() {
		Debug.Log("Analytics called!");
//        mainActivity.runOnUiThread(new Runnable() {
//            public void run() {
//                Native.mainActivity.analytics.showAppInstanceIdPopup();
//            }
//        });
	}

	public static boolean showInterstitialAd(double var0) {
//        if (mainActivity.gameServices != null && mainActivity.gameServices.adsHelper != null) {
//            if (!mainActivity.gameServices.adsHelper.canShowInterstitial()) {
//                Debug.Log("Native.showInterstitialAd no ad ready");
//                return false;
//            } else {
//                mainActivity.runOnUiThread(new Runnable(var0) {
//                    final double val$delay;
//
//                    {
//                        this.val$delay = var1;
//                    }
//
//                    public void run() {
//                        StringBuilder var1 = new StringBuilder("Native.showInterstitialAd(");
//                        var1.append(this.val$delay);
//                        var1.append(") called from native");
//                        Debug.Log(var1.toString());
//                        (new Handler()).postDelayed(new Runnable(this) {
//                            final <undefinedtype> this$0;
//
//                            {
//                                this.this$0 = var1;
//                            }
//
//                            public void run() {
//                                if (Native.mainActivity.gameServices != null && Native.mainActivity.gameServices.adsHelper != null) {
//                                    Native.mainActivity.gameServices.adsHelper.displayInterstitial();
//                                }
//
//                            }
//                        }, (long)((int)(this.val$delay * 1000.0)));
//                    }
//                });
//                return true;
//            }
//        } else {
//            return false;
//        }
		return false;
	}

	public static void showLeaderboards() {
	}

	public static void showPlatformConsentOptions() {
//        mainActivity.runOnUiThread(new Runnable() {
//            public void run() {
//                Native.mainActivity.gameServices.adsHelper.showConsentForm();
//            }
//        });
	}

	public static native void snapshotLoaded(String var0, byte[] var1);

	public static void startAdsAndAnalytics() {
//        mainActivity.runOnUiThread(new Runnable() {
//            public void run() {
//                label17: {
//                    if (!Native.isAgeOfConsent()) {
//                        Debug.Log("Native.startAdsAndAnalytics: not starting analytics because underage");
//                    } else {
//                        if (Native.mainActivity.gameServices.adsHelper.getCurrentConsentStatus() == ConsentStatus.NotRequired) {
//                            Debug.Log("Native.startAdsAndAnalytics: starting analytics");
//                            Native.mainActivity.analytics.start();
//                            break label17;
//                        }
//
//                        Debug.Log("Native.startAdsAndAnalytics: not starting analytics because platform consent required");
//                    }
//
//                    if (Native.isAgeOfConsent()) {
//                        RemoteConfig.getInstance().startRemote();
//                    }
//                }
//
//                Debug.Log("Native.startAdsAndAnalytics: starting ads");
//                Native.mainActivity.gameServices.adsHelper.start();
//            }
//        });
	}

	public static boolean startReviewFlow() {
		Debug.Log("Start review flow");
//        mainActivity.runOnUiThread(() -> {
//                Native.mainActivity.gameServices.startReviewFlow();
//        });
		return false;
	}

	public static void startTextInput(String var0) {
		MainActivity mainActivity = getActivity();
		if (mainActivity == null) return;

		mainActivity.runOnUiThread(() -> mainActivity.StartGettingTextInput(var0));
	}

	public static native void startedRestoringPurchases();

	public static void stopTextInput() {
		MainActivity mainActivity = getActivity();
		if (mainActivity == null) return;

		mainActivity.runOnUiThread(mainActivity::StopGettingTextInput);
	}

	public static native void storeProductFetchFailed(String var0, String var1);

	public static native void storeProductFetched(String var0, String var1, String var2);

	public static native void textInputDidFinish();

	public static native void textInputTextDidChange(String var0);

	public static native String uniqueIdentifier();

	public static native void updateApplication(float var0);

	public static void withdrawPrivacyConsent() {
		Debug.Log("Withdraw privacy consent");
		saveBooleanInSP("privacyConsent", false);
		saveBooleanInSP("explicitConsent", true);
	}
}

