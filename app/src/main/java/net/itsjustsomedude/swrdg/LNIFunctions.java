package net.itsjustsomedude.swrdg;

import android.util.Log;

import com.touchfoo.swordigo.GameTime;
import com.touchfoo.swordigo.Native;

public class LNIFunctions {
    private static final String TAG = "LNIFunctions";

    public static void openUrl(String url, boolean askFirst) {
        Native.openURL(url);
    }

    public static void copyToClipboard(String title, String content) {
        Native.copyToClipboard(title, content);
    }

    public static void setSpeed(double speed) {
        GameTime.scaling = Math.clamp(speed, 0.05, 16);
    }

    public static void quit() {
        MainActivity mainActivity = Native.getActivity();
        if (mainActivity == null) return;

        mainActivity.finish();
    }

    public static void spit() {
        Log.d(TAG, "I am printed from Java, from Lua!");
    }

    public static String spitReturn() {
        Log.d(TAG, "I am printed from Java, from Lua! And now I return something?");
        return "Returned from Java";
    }

    static {
//        LuaNativeInterface.addMethod(LNIFunctions.class, "openUrl");
//        LuaNativeInterface.addMethod(LNIFunctions.class, "copyToClipboard");
//        LuaNativeInterface.addMethod(LNIFunctions.class, "setSpeed");
//        LuaNativeInterface.addMethod(LNIFunctions.class, "quit");
    }
}
