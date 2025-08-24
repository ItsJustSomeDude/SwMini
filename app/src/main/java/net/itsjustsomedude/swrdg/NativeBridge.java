package net.itsjustsomedude.swrdg;

import android.content.Context;

import com.touchfoo.swordigo.Debug;

import java.io.File;
import java.io.InputStream;
import java.util.Properties;

public class NativeBridge {

    public static native void prepareHooks();
    public static native void cleanupHooks();

    public static native void addStringReplacement(String source, String replacement);

    public static native void setPaths(String files, String cache, String extFiles, String extCache);

    public static native void init();

    public static String processInJava(String input) {
        return "Java processed: " + input;
    }

    public static void setupCStringHooks(Context ctx) {
        Debug.Log("Setting up CStrings.");

        Properties props = new Properties();
        try {
            InputStream inputStream = ctx.getAssets().open("cstrings.properties");
            props.load(inputStream);
            inputStream.close();
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }

        for (String key : props.stringPropertyNames()) {
            String unescapedKey = key
                    .replaceAll("\\\\_", "\255")
                    .replaceAll("_", " ")
                    .replaceAll("\255", "_");

            String value = props.getProperty(key);
            addStringReplacement(unescapedKey, value);
            Debug.Log("Setup Replacement: '" + unescapedKey + "' ('" + key + "') -> '" + value + "'");
        }
    }

    public static void setupFilePaths(Context ctx) {
        File ext = ctx.getExternalFilesDir(null);
        String e = null;
        if(ext != null)
            e = ext.getAbsolutePath() + "/%s";

        File extCache = ctx.getExternalCacheDir();
        String c = null;
        if(extCache != null)
            c = extCache.getAbsolutePath() + "/%s";

        setPaths(
                ctx.getFilesDir().getAbsolutePath() + "/%s",
                ctx.getCacheDir().getAbsolutePath() + "/%s",
                e, c
        );
    }

    public static void processLni(String message) {
        Debug.Log("Received LNI from C");
        try {
            LNIString.execute(message);
		} catch(Exception e) {
			Debug.Log("LNI Failure:", e);
		}
    }

    public static void loadLibraries(Context context) {
        File libDir = new File(context.getApplicationInfo().nativeLibraryDir);
        File[] files = libDir.listFiles();
        if(files == null) return;

        for (File file : files) {
            String name = file.getName();
            if(!name.endsWith(".so")
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
