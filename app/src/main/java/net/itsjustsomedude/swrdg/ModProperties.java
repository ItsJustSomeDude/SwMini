package net.itsjustsomedude.swrdg;

import android.content.Context;

import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;

import io.github.wasabithumb.jtoml.JToml;
import io.github.wasabithumb.jtoml.document.TomlDocument;
import io.github.wasabithumb.jtoml.key.TomlKey;
import io.github.wasabithumb.jtoml.value.TomlValue;
import io.github.wasabithumb.jtoml.value.primitive.TomlPrimitive;
import io.github.wasabithumb.jtoml.value.primitive.TomlPrimitiveType;
import io.github.wasabithumb.jtoml.value.table.TomlTable;

public class ModProperties {
	public static final Map<String, String> links = new HashMap<>();
	public static boolean googleVisible = false;
	public static boolean pauseLostFocus = true;
	public static boolean show32bitPopup = true;
	public static String overlayModName = "";
	public static String overlayModVersion = "";
	public static String overlayModAuthors = "";
	public static String overlayModReadme = "";

	public static void loadDefaultPropertiesFile(Context ctx) {
		try {
			JToml jtoml = JToml.jToml();
			InputStream inputStream = ctx.getAssets().open("mini.toml");
			TomlDocument doc = jtoml.read(inputStream);

			inputStream.close();
			loadProperties(doc);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static void loadProperties(TomlTable tbl) {
		// We need to read [mod_overlay], [links], and a couple [options]

		overlayModName = getString(tbl, "mod_overlay.name", overlayModName);
		overlayModVersion = getString(tbl, "mod_overlay.version", overlayModVersion);
		overlayModReadme = getString(tbl, "mod_overlay.name", overlayModReadme);

		TomlValue linksV = tbl.get("links");
		if (linksV != null && linksV.isTable()) {
			TomlTable t = linksV.asTable();

			for (TomlKey a : t.keys()) {
				TomlValue v = t.get(a);
				TomlPrimitive p;
				if (v != null &&
					v.isPrimitive() &&
					(p = v.asPrimitive()).type() == TomlPrimitiveType.STRING
				) {
					links.put(a.toString(), p.asString());
				}
			}
		}

		googleVisible = getBoolean(tbl, "options.show_google_button", googleVisible);
		show32bitPopup = getBoolean(tbl, "options.32bit_popup", show32bitPopup);
		pauseLostFocus = getBoolean(tbl, "options.pauseOnLostFocus", pauseLostFocus);
	}

	private static String getString(TomlTable tbl, CharSequence key, String defaultVal) {
		TomlValue v = tbl.get(key);
		TomlPrimitive p;
		if (v != null
			&& v.isPrimitive()
			&& (p = v.asPrimitive()).type() == TomlPrimitiveType.STRING
		) {
			return p.asString();
		}
		return defaultVal;
	}

	private static boolean getBoolean(TomlTable tbl, CharSequence key, boolean defaultVal) {
		TomlValue v = tbl.get(key);
		TomlPrimitive p;
		if (v != null
			&& v.isPrimitive()
			&& (p = v.asPrimitive()).type() == TomlPrimitiveType.BOOLEAN
		) {
			return p.asBoolean();
		}
		return defaultVal;
	}

	private static int getInt(TomlTable tbl, CharSequence key, int defaultVal) {
		TomlValue v = tbl.get(key);
		if (v != null && v.isPrimitive() && v.asPrimitive().type() == TomlPrimitiveType.INTEGER) {
			return v.asPrimitive().asInteger();
		}
		return defaultVal;
	}

	private static double getDouble(TomlTable tbl, CharSequence key, double defaultVal) {
		TomlValue v = tbl.get(key);
		if (v != null && v.isPrimitive() && v.asPrimitive().type() == TomlPrimitiveType.FLOAT) {
			return v.asPrimitive().asDouble();
		}
		return defaultVal;
	}
}
