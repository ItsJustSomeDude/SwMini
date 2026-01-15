package net.itsjustsomedude.swrdg;

import android.content.Context;

import com.touchfoo.swordigo.GameTime;

import java.io.InputStream;
import java.util.Properties;

public class ModProperties {
	public static String facebookLink = "openUrl(\"https://files.itsjustsomedu.de/\", false);";
	public static String twitterLink = "openUrl(\"https://discord.com/invite/3FhQvTUKA4\", false);";
	public static String privacyLink = "openUrl(\"https://discord.com/invite/3FhQvTUKA4\", false);";
	public static double defaultSpeed = 1;

	public static boolean googleVisible = false;
	public static String googleAction = "openUrl(\"https://google.com/\", false);";

	public static boolean pauseLostFocus = true;

	public static String modName = "";
	public static String modVersion = "";
	public static String modAuthors = "";

	public static int defaultCoinLimit = 999;
	public static int tooRichAmount = 999;

	public static void loadDefaultPropertiesFile(Context ctx) {
		Properties props = new Properties();
		try {
			InputStream inputStream = ctx.getAssets().open("mini.properties");
			props.load(inputStream);
			inputStream.close();
		} catch (Exception e) {
			e.printStackTrace();
			return;
		}
		loadProperties(props);
	}

	public static void loadProperties(Properties props) {
		facebookLink = props.getProperty("links.facebook", facebookLink);
		twitterLink = props.getProperty("links.twitter", twitterLink);
		privacyLink = props.getProperty("links.privacy", privacyLink);

		googleVisible = getBoolean(props, "google.visible", googleVisible);
		googleAction = props.getProperty("google.action", googleAction);


		modName = props.getProperty("mod.name", modName);
		modVersion = props.getProperty("mod.version", modVersion);
		modAuthors = props.getProperty("mod.authors", modAuthors);

		defaultSpeed = getDouble(props, "defaults.speed", defaultSpeed);
		GameTime.scaling = defaultSpeed;

		defaultCoinLimit = getInt(props, "defaults.coinLimit", defaultCoinLimit);
		NativeBridge.setDefaultCoinLimit(defaultCoinLimit);

		pauseLostFocus = getBoolean(props, "options.pauseOnLostFocus", pauseLostFocus);

		tooRichAmount = getInt(props, "options.tooRichAmount", tooRichAmount);
		NativeBridge.setTooRichAmount(tooRichAmount);
	}

	private static int getInt(Properties props, String key, int defaultVal) {
		try {
			String v = props.getProperty(key);
			if (v == null) return defaultVal;
			return Integer.parseInt(v);
		} catch (NumberFormatException | NullPointerException ignored) {
			return defaultVal;
		}
	}

	private static double getDouble(Properties props, String key, double defaultVal) {
		try {
			String v = props.getProperty(key);
			if (v == null) return defaultVal;
			return Double.parseDouble(v);
		} catch (NumberFormatException | NullPointerException ignored) {
			return defaultVal;
		}
	}

	private static boolean getBoolean(Properties props, String key, boolean defaultVal) {
		String v = props.getProperty(key);
		if (v == null) return defaultVal;

		if ("true".equalsIgnoreCase(v)) return true;
		else if ("false".equalsIgnoreCase(v)) return false;
		else return defaultVal;
	}
}
