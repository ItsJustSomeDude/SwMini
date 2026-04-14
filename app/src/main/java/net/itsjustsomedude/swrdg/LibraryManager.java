package net.itsjustsomedude.swrdg;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.os.Build;

import com.touchfoo.swordigo.Debug;

import java.io.File;

public class LibraryManager {
	private static native void setMiniAssetManager(AssetManager manager);

	private static native boolean nativePreloadLibrary(String libPath);

	private static native boolean nativePreloadEngine(String libPath);

	private static native void setMiniFilePaths(
		String files, String cache, String extFiles, String extCache
	);


	/**
	 * This centralizes all of the initialization that Mini needs to do into one function.
	 * It loads the library, sets asset manager, sets Paths, and registers the native crash handler.
	 */
	public static void loadMini(Context ctx) {
		System.loadLibrary("mini");

		setMiniAssetManager(ctx.getAssets());

		File extFilesDir = ctx.getExternalFilesDir(null);
		String extFiles = (extFilesDir != null) ? extFilesDir.getAbsolutePath() : null;
		File extCacheDir = ctx.getExternalCacheDir();
		String extCache = (extCacheDir != null) ? extCacheDir.getAbsolutePath() : null;
		String files = ctx.getFilesDir().getAbsolutePath();
		String cache = ctx.getCacheDir().getAbsolutePath();
		setMiniFilePaths(files, cache, extFiles, extCache);

		// Disable crash handler on Debug builds, because it muddies stack traces.
		if (!BuildConfig.DEBUG)
			CrashHandler.setupCrashHandler(ctx);
	}

	public static boolean preloadEngine(Context ctx) {
		PackageManager pm = ctx.getPackageManager();

		try {
			PackageInfo pkg = pm.getPackageInfo("com.touchfoo.swordigo", 0);
			if (pkg.applicationInfo == null) {
				Debug.Log("Failed to locate Vanilla package.");
				return false;
			}

			String abi = getPrimaryAbi();
			if (abi == null) return false;

			String libDir = pkg.applicationInfo.sourceDir + "!/lib/" + abi + "/";
			Debug.Log("Attempting to preload engine libraries from " + libDir);

			nativePreloadLibrary(libDir + "libopenal-soft.so");
			nativePreloadEngine(libDir + "libswordigo.so");

			return true;
		} catch (PackageManager.NameNotFoundException e) {
			return false;
		}
	}

	public static String getPrimaryAbi() {
		// This is only "constant" because BuildConfig is considered constant, but it's not.
		// noinspection ConstantValue
		if ("debug32".equals(BuildConfig.BUILD_TYPE))
			return "armeabi-v7a";

		for (String abi : Build.SUPPORTED_ABIS) {
			// Find the first matching supported ABI
			if (abi.equals("arm64-v8a")) {
				return abi;
			} else if (abi.equals("armeabi-v7a")) {
				return abi;
			}
		}
		return null;
	}
}
