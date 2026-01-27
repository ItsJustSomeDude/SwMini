package net.itsjustsomedude.swrdg;

import android.content.Context;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Properties;

public class Achievements {
	private static final String LOG_TAG = "MiniAchJava";

	private static final LinkedHashMap<String, Achievement> achievements = new LinkedHashMap<>();

	public static void readDefaultFile(Context ctx) {
		try {
			InputStream inputStream = ctx.getAssets().open("achievements.properties");

			readAchievementsProperties(getOrderedProperties(inputStream));

			inputStream.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static void readAchievementsProperties(Map<String, String> raw) {
		for (String k : raw.keySet()) {
			String[] splitKey = k.split("\\.", 2);
			if (splitKey.length != 2) {
				throw new IllegalArgumentException("Bad achievement! Invalid key: " + k);
			}

			String id = splitKey[0].toLowerCase();
			String prop = splitKey[1].toLowerCase();
			String strVal = raw.get(k);

			if (strVal == null) {
				Log.e(LOG_TAG, "Value is null.");
				continue;
			}

			Achievement current;
			if (achievements.containsKey(id)) {
				current = achievements.get(id);
			} else {
				current = new Achievement();
				achievements.put(id, current);
				current.id = id;
			}
			if (current == null) {
				// This should be impossible.
				continue;
			}

			switch (prop) {
				case "name":
					current.name = strVal;
					break;
				case "description":
				case "desc":
					current.description = strVal;
					break;
				case "counter":
					current.counter = strVal;
					break;
				case "points":
				case "pts":
					// Handle error.
					current.points = Integer.parseInt(strVal);
					break;
				case "threshold":
					// Handle error.
					current.threshold = Integer.parseInt(strVal);
					break;
			}
		}
	}

	public static void registerAll() {
		for (Achievement ach : achievements.values()) {
			if (ach.id == null) {
				Log.e(LOG_TAG, "Failed to register achievement! Missing ID.");
				return;
			}
			if (ach.name == null) {
				Log.e(LOG_TAG, "Failed to register achievement! Missing Name for " + ach.id);
				return;
			}

			NativeBridge.createAchievement(
				ach.id, ach.name, ach.description, ach.points,
				ach.counter, ach.threshold
			);
		}
	}

	private static Map<String, String> getOrderedProperties(InputStream in) throws IOException {
		Map<String, String> mp = new LinkedHashMap<>();
		(new Properties() {
			public synchronized Object put(Object key, Object value) {
				return mp.put((String) key, (String) value);
			}
		}).load(in);
		return mp;
	}

	private static class Achievement {
		public String id = null;
		public String name = null;
		public String description = "";
		public String counter = null;
		public int points = 10;
		public int threshold = 1;

		public Achievement() {
		}
	}
}
