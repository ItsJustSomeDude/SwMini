package net.itsjustsomedude.swrdg;

import android.content.Intent;
import android.net.Uri;

import com.touchfoo.swordigo.Debug;

import java.lang.ref.WeakReference;

public class GameActions {
	public static WeakReference<MainActivity> mainActivityRef;

	public static MainActivity getActivity() {
		if (mainActivityRef == null) return null;
		return mainActivityRef.get();
	}

	public static void openURL(String url) {
		if (url == null) return;
		MainActivity mainActivity = getActivity();
		if (mainActivity == null) return;

		if (url.startsWith("overlay:")) {
			mainActivity.runOnUiThread(MiniOverlay::show);
			return;
		}

		mainActivity.startActivity(
			new Intent("android.intent.action.VIEW", Uri.parse(url))
		);
	}

	public static void deleteSnapshot(String var0) {
//		MainActivity mainActivity = getActivity();
//		if (mainActivity == null) return;
//
//		mainActivity.runOnUiThread(() -> {
//			StringBuilder var1 = new StringBuilder();
//			var1.append(this.val$name);
//			Debug.Log("Sync: deleteSnapshot called from native: " + var0);
//			mainActivity.gameServices.deleteSnapshot(this.var0);
//		});
	}

	public static void loadSnapshot(String name, double delay) {
//		MainActivity mainActivity = getActivity();
//		if (mainActivity == null) return;
//
//        mainActivity.runOnUiThread(() -> {
//			Debug.Log("Sync: loadSnapshot called from native: " + name + ", " + delay);
//
//			(new Handler()).postDelayed(() -> {
//				if (name != null && !name.isEmpty()) {
//					mainActivity.gameServices.loadSnapshot(name);
//				} else {
//					mainActivity.gameServices.loadAllSnapshots();
//				}
//			}, delay * 1000);
//		});
	}

	public static void saveSnapshot(String name, byte[] data, String description, long timePlayedMillis, long progressValue) {
		Debug.Log("Save Snapshot called!");
//		MainActivity mainActivity = getActivity();
//		if (mainActivity == null) return;
//
//		mainActivity.runOnUiThread(() -> {
//			Debug.Log(
//				"Sync: saveSnapshot called from native: " +
//					name + ", " +
//					data.length + ", " +
//					description + ", " +
//					timePlayedMillis + ", " +
//					progressValue
//			);
//
//			mainActivity.gameServices.saveSnapshot(name, data, description, timePlayedMillis, progressValue);
//		});
	}
}
