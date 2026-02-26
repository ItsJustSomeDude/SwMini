package net.itsjustsomedude.swrdg;

import android.content.Context;
import android.content.res.AssetManager;

import java.io.File;

public class LibraryManager {
	private static native void setMiniAssetManager(AssetManager manager);

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
		
		// if (!BuildConfig.DEBUG)
		CrashHandler.setupCrashHandler(ctx);
	}
}

/*
 * Eventually, this may be used to help to help with loading libraries from Vanilla.
 */

//		String packageName = "com.touchfoo.swordigo";
//		PackageManager pm = this.getPackageManager();
//
//		try {
//			PackageInfo pkg = pm.getPackageInfo(packageName, 0);
//			if (pkg.applicationInfo != null) {
//				Debug.Log("Is this it? " + pkg.applicationInfo.sourceDir);
//			}
//		} catch (PackageManager.NameNotFoundException e) {
//			e.printStackTrace();
//		}
