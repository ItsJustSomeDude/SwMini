package net.itsjustsomedude.swrdg;

import android.content.Context;
import android.content.res.AssetManager;

import java.io.File;

public class NativeBridge {
	private static final String LOG_TAG = "MiniNativeBridge";

	public static native void setPaths(String files, String cache, String extFiles, String extCache);

	public static native void setMiniAssetManager(AssetManager manager);

	public static native void midLoad();

	public static native void lateLoad();

	public static void setupFilePaths(Context ctx) {
		File ext = ctx.getExternalFilesDir(null);
		String e = null;
		if (ext != null)
			e = ext.getAbsolutePath() + "/";

		File extCache = ctx.getExternalCacheDir();
		String c = null;
		if (extCache != null)
			c = extCache.getAbsolutePath() + "/";

		setPaths(
			ctx.getFilesDir().getAbsolutePath() + "/",
			ctx.getCacheDir().getAbsolutePath() + "/",
			e, c
		);
	}

	public static void loadLibraries(Context context) {
		// TODO: This is Broken. The folder cannot be enumerated anymore. Must use a list defined in
		// mini.properties and try loading each one, catching UnsatisfiedLinkError if it's invalid.

		File libDir = new File(context.getApplicationInfo().nativeLibraryDir);
		File[] files = libDir.listFiles();
		if (files == null) return;

		for (File file : files) {
			String name = file.getName();
			if (!name.endsWith(".so")
				|| name.equals("libswordigo.so")
				|| name.equals("libopenal-soft.so")
				|| name.equals("libmini.so")
				|| name.equals("libGlossHook.so")
			) continue;

			String libName;
			if (name.toLowerCase().startsWith("lib"))
				libName = name.substring(3, name.length() - 3);
			else
				libName = name.substring(0, name.length() - 3);

			System.loadLibrary(libName);
		}
	}
}
