package com.touchfoo.swordigo;

import android.util.Log;

public class Debug {
	public static final boolean loggingEnabled = true;

	public static void Log(String toLog) {
		if (loggingEnabled) {
			Log.d("Swordigo", toLog);
		}
	}

	public static void Log(String toLog, Exception throwable) {
		if (loggingEnabled) {
			Log.d("Swordigo", toLog, throwable);
		}
	}
}
