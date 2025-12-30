package net.itsjustsomedude.swrdg;

import com.touchfoo.swordigo.GameTime;
import com.touchfoo.swordigo.Native;

/**
 * @noinspection unused
 */
public class LNIFunctions {
	private static final String TAG = "LNIFunctions";

	public static void register() {
		LuaNativeInterface.registerMethod(LNIFunctions.class, "openUrl");
		LuaNativeInterface.registerMethod(LNIFunctions.class, "copyToClipboard");
		LuaNativeInterface.registerMethod(LNIFunctions.class, "setSpeed");
		LuaNativeInterface.registerMethod(LNIFunctions.class, "getSpeed");
		LuaNativeInterface.registerMethod(LNIFunctions.class, "quit");

		LuaNativeInterface.registerMethod(LNIFunctions.class, "returnTest1");
		LuaNativeInterface.registerMethod(LNIFunctions.class, "argTest1");
		LuaNativeInterface.registerMethod(LNIFunctions.class, "errorTest1");
		LuaNativeInterface.registerMethod(LNIFunctions.class, "errorTest2");
	}

	public static void openUrl(String url) {
		Native.openURL(url);
	}
//    public static void askOpenUrl(String url, boolean askFirst) {}

	public static void copyToClipboard(String content) {
		Native.copyToClipboard("Swordigo", content);
	}

	public static double getSpeed() {
		return GameTime.scaling;
	}

	public static void setSpeed(double speed) {
		GameTime.scaling = Math.clamp(speed, 0.05, 16);
	}

	public static void quit() {
		MainActivity mainActivity = Native.getActivity();
		if (mainActivity == null) return;

		mainActivity.finish();
	}

	public static String returnTest1() {
		return "String from Java";
	}

	public static String argTest1(String arg1, String arg2) {
		return "Args: " + arg1 + ", " + arg2;
	}

	public static String errorTest1() throws Exception {
		throw new Exception("Intentional Java Error.");
	}

	public static String errorTest2(String arg1) throws Exception {
		throw new Exception("Intentional Java Error: " + arg1);
	}
}
