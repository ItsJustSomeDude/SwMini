package com.touchfoo.swordigo;

import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.AssetManager;

import net.itsjustsomedude.swrdg.GameActions;
import net.itsjustsomedude.swrdg.MainActivity;
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
		GameActions.deleteSnapshot(var0);
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

	// NOT called from native!
	public static boolean getBooleanFromSP(String var0) {
		MainActivity mainActivity = getActivity();
		if (mainActivity == null) return false;

		return mainActivity.getApplicationContext().getSharedPreferences("SwordigoPreferences", 0).getBoolean(var0, false);
	}

	// NOT called from native!
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
		GameActions.openURL(
			ModProperties.links.getOrDefault("google", "overlay:")
		);
//        mainActivity.runOnUiThread(new Runnable() {
//            public void run() {
//                Debug.Log("initiateGoogleSignIn called from native");
//                if (Native.mainActivity.gamesSignIn != null) {
//                    Native.mainActivity.gamesSignIn.beginInteractiveSignIn();
//                }
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
//		MainActivity mainActivity = getActivity();
//		if (mainActivity == null) return;
//
//		mainActivity.runOnUiThread(() -> {
//			Debug.Log("Sync: loadInterstitialAd() called from native");
//			if (mainActivity.gameServices != null && mainActivity.gameServices.adsHelper != null) {
//				mainActivity.gameServices.adsHelper.loadInterstitialIfNecessary();
//			}
//		});
	}

	public static void loadSnapshot(String var0, double var1) {
		GameActions.loadSnapshot(var0, var1);
	}

	public static void openURL(String url) {
		Debug.Log("Open URL: " + url);

		final String twitter = "http://www.twitter.com/touch_foo";
		final String facebook = "http://www.facebook.com/144881932264830";
		final String privacy = "http://privacy.touchfoo.com/";

		if (Objects.equals(url, twitter)) {
			GameActions.openURL(ModProperties.links.getOrDefault("twitter", twitter));
		} else if (Objects.equals(url, facebook)) {
			GameActions.openURL(ModProperties.links.getOrDefault("facebook", facebook));
		} else if (Objects.equals(url, privacy)) {
			GameActions.openURL(ModProperties.links.getOrDefault("privacy", privacy));
		} else {
			GameActions.openURL(url);
		}
	}

	public static void prepareReviewFlow() {
	}

	public static void processStoreQueue() {
//		MainActivity mainActivity = getActivity();
//		if (mainActivity == null) return;
//
//		mainActivity.runOnUiThread(() -> {
//			mainActivity.storeController.processQueue();
//		});
	}

	public static native void productPurchaseFailed(String var0, String var1);

	public static native void productPurchased(String var0);

	public static void purchaseStoreProduct(String identifier) {
//		MainActivity mainActivity = getActivity();
//		if (mainActivity == null) return;
//
//		mainActivity.runOnUiThread(() -> {
//			mainActivity.storeController.purchaseProduct(mainActivity, identifier);
//		});
	}

	public static void queueStoreProductFetch(String identifier) {
		// Unsure if I added the debug call or that's vanilla...
		Debug.Log("Store Product Fetch called!");
		debugFunction();

//		MainActivity mainActivity = getActivity();
//		if (mainActivity == null) return;
//
//		mainActivity.runOnUiThread(() -> {
//			mainActivity.storeController.queueProductFetch(identifier);
//		});
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

	public static void reportAchievementProgress(String identifier, int stepsCompleted, boolean isIncremental) {
		Debug.Log("Report Achievements called!");

//		MainActivity mainActivity = getActivity();
//		if (mainActivity == null) return;
//
//		mainActivity.runOnUiThread(() -> {
//			Debug.Log("reportAchievementProgress(" + identifier + ", " + stepsCompleted + ", " + isIncremental + ") called from native");
//			mainActivity.gameServices.reportAchievementProgress(identifier, stepsCompleted, isIncremental);
//		});
	}

	public static native void reviewFlowCompleted();

	// NOT called from native!
	public static void saveBooleanInSP(String var0, boolean var1) {
		MainActivity mainActivity = getActivity();
		if (mainActivity == null) return;

		SharedPreferences.Editor var2 = mainActivity.getApplicationContext().getSharedPreferences("SwordigoPreferences", 0).edit();
		var2.putBoolean(var0, var1);
		var2.apply();
	}

	// NOT called from native!
	public static void saveIntInSP(String var0, int var1) {
		MainActivity mainActivity = getActivity();
		if (mainActivity == null) return;

		SharedPreferences.Editor var2 = mainActivity.getApplicationContext().getSharedPreferences("SwordigoPreferences", 0).edit();
		var2.putInt(var0, var1);
		var2.apply();
	}

	public static void saveSnapshot(String name, byte[] data, String description, long timePlayedMillis, long progressValue) {
		Debug.Log("Save Snapshot called!");

		GameActions.saveSnapshot(name, data, description, timePlayedMillis, progressValue);
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

//		MainActivity mainActivity = getActivity();
//		if (mainActivity == null) return;
//
//		mainActivity.runOnUiThread(() -> {
//			Debug.Log("showAchievements called from native");
//			mainActivity.gameServices.showAchievements();
//		});
	}

	public static void showAnalyticsIdPopup() {
		Debug.Log("Analytics called!");

//		MainActivity mainActivity = getActivity();
//		if (mainActivity == null) return;
//
//		mainActivity.runOnUiThread(() -> {
//			Debug.Log("showAchievements called from native");
//			mainActivity.analytics.showAppInstanceIdPopup();
//		});
	}

	public static boolean showInterstitialAd(double delay) {
		return false;

//		MainActivity mainActivity = getActivity();
//		if (mainActivity == null) return false;
//
//		if (mainActivity.gameServices == null || mainActivity.gameServices.adsHelper == null)
//			return false;
//
//		if (!mainActivity.gameServices.adsHelper.canShowInterstitial()) {
//			Debug.Log("Native.showInterstitialAd no ad ready");
//			return false;
//		}
//
//		mainActivity.runOnUiThread(() -> {
//			Debug.Log("Native.showInterstitialAd(" + delay + ") called from native");
//
//			(new Handler()).postDelayed(() -> {
//				if (mainActivity.gameServices == null || mainActivity.gameServices.adsHelper == null)
//					return;
//				mainActivity.gameServices.adsHelper.displayInterstitial();
//			}, delay * 1000.0);
//		});
//
//		return true;
	}

	// NOT called from native!
	public static void showLeaderboards() {
	}

	public static void showPlatformConsentOptions() {
//		MainActivity mainActivity = getActivity();
//		if (mainActivity == null) return;
//
//		mainActivity.runOnUiThread(() -> {
//			mainActivity.gameServices.adsHelper.showConsentForm();
//		});
	}

	public static native void snapshotLoaded(String var0, byte[] var1);

	public static void startAdsAndAnalytics() {
//		MainActivity mainActivity = getActivity();
//		if (mainActivity == null) return;
//
//		mainActivity.runOnUiThread(() -> {
//			label17:
//			{
//				if (!Native.isAgeOfConsent()) {
//					Debug.Log("Native.startAdsAndAnalytics: not starting analytics because underage");
//				} else {
//					if (mainActivity.gameServices.adsHelper.getCurrentConsentStatus() == ConsentStatus.NotRequired) {
//						Debug.Log("Native.startAdsAndAnalytics: starting analytics");
//						mainActivity.analytics.start();
//						break label17;
//					}
//
//					Debug.Log("Native.startAdsAndAnalytics: not starting analytics because platform consent required");
//				}
//
//				if (Native.isAgeOfConsent()) {
//					RemoteConfig.getInstance().startRemote();
//				}
//			}
//
//			Debug.Log("Native.startAdsAndAnalytics: starting ads");
//			mainActivity.gameServices.adsHelper.start();
//		});
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

