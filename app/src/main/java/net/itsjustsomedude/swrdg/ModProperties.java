package net.itsjustsomedude.swrdg;

import android.content.Context;

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

    public static void loadProperties(Context ctx) {
        Properties props = new Properties();
        try {
            InputStream inputStream = ctx.getAssets().open("mini.properties");
            props.load(inputStream);
            inputStream.close();
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }

        facebookLink = props.getProperty("links.facebook", facebookLink);
        twitterLink = props.getProperty("links.twitter", twitterLink);
        privacyLink = props.getProperty("links.privacy", privacyLink);

        try {
            defaultSpeed = Double.parseDouble(props.getProperty("defaults.speed", "1"));
        } catch (NumberFormatException | NullPointerException ignored) {}

        googleVisible = Boolean.parseBoolean(props.getProperty("google.visible", "true"));
        googleAction = props.getProperty("google.action", googleAction);

        pauseLostFocus = Boolean.parseBoolean(props.getProperty("options.pauseOnLostFocus", "true"));

        modName = props.getProperty("mod.name", modName);
        modVersion = props.getProperty("mod.version", modVersion);
        modAuthors = props.getProperty("mod.authors", modAuthors);
    }
}
