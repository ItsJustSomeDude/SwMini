package net.itsjustsomedude.swrdg;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.util.Log;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.text.DateFormat;
import java.util.Calendar;

public class CrashHandler {
	private static final String TAG = "MiniCrashHandler";
	private static final String NATIVE_MARKER = "native_crash_marker";
	static Thread.UncaughtExceptionHandler defaultHandler;

	/**
	 * For this to succeed, libmini must already be loaded and available for JNI.
	 * <p>
	 * This will clear any pending crashes, because the `open` call has the Truncate flag set.
	 */
	static void setupCrashHandler(Context ctx) {
		// TODO: Some way to catch Java crashes well?
//		defaultHandler = Thread.getDefaultUncaughtExceptionHandler();
//		Thread.setDefaultUncaughtExceptionHandler((Thread thread, Throwable e) -> {
//			Log.e(TAG, "Caught a Java crash!", e);
//			defaultHandler.uncaughtException(thread, e);
//		});

		String markerPath = ctx.getCacheDir() + "/" + NATIVE_MARKER;
		setupNativeCrashHandler(markerPath);
	}

	static native void setupNativeCrashHandler(String path);

	static void showReportDialog(Context ctx, boolean isCrash) {
		int title = isCrash ? R.string.crash_report_title : R.string.report_title;
		int body = isCrash ? R.string.crash_report_body : R.string.crash_report_title;

		new AlertDialog.Builder(ctx)
			.setTitle(title)
			.setMessage(body)
			.setPositiveButton(R.string.report_logs, (DialogInterface dialog, int which) -> {
			})
			.setNegativeButton(R.string.report_cancel, (DialogInterface dialog, int which) -> {
			})
			.setNeutralButton(R.string.report_export, (DialogInterface dialog, int which) -> {
			})
			.setOnDismissListener((DialogInterface dialog) -> {
			})
			.show();
	}

	/**
	 * Check the cache for a crash marker file.
	 */
	static boolean lastRunCrashed(Context ctx) {
		String markerPath = ctx.getCacheDir() + "/" + NATIVE_MARKER;
		File markerFile = new File(markerPath);

		StringBuilder stringBuilder = new StringBuilder();
		try (BufferedReader reader = new BufferedReader(new FileReader(markerFile))) {
			String line;
			// Read each line from the file
			while ((line = reader.readLine()) != null)
				stringBuilder.append(line);
		} catch (IOException ignored) {
		}

		return stringBuilder.toString().startsWith("crash");
	}

	/**
	 * Remove the crash marker from the cache. This usually does not need to be done, because
	 * starting the native crash handler `TRUNC`ates the marker.
	 */
	static void clearCrashes(Context ctx) {
	}

	/**
	 * Dumps logs to a file, returning the path.
	 */
	static String dumpLogs(Context ctx) {
		String filePrefix =
			(ModProperties.overlayModName == null || ModProperties.overlayModName.isEmpty())
				? "SwMini"
				: ModProperties.overlayModName;
		String fileName = (filePrefix + "_" +
			DateFormat.getInstance().format(Calendar.getInstance().getTime()))
			.replace(' ', '_')
			.replace('.', '_')
			.replace('/', '-') + ".log";

		File targetDir = new File(ctx.getCacheDir(), LogProvider.LOG_SUBDIR);
		if (!targetDir.exists() && !targetDir.mkdirs()) return null;

		File outFile = new File(targetDir, fileName);

		Process process = null;
		BufferedReader reader = null;
		FileWriter writer = null;

		try {
			// -d = dump logs and exit
			// -v threadtime = readable timestamps + thread info (optional but useful)
			process = new ProcessBuilder("logcat", "-d", "-v", "threadtime")
				.redirectErrorStream(true)
				.start();

			reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
			writer = new FileWriter(outFile, false);

			String line;
			while ((line = reader.readLine()) != null) {
				writer.write(line);
				writer.write('\n');
			}

			writer.flush();
		} catch (IOException e) {
			Log.e(TAG, "Error during log read process!", e);
			return null;
		} finally {
			try {
				if (reader != null) reader.close();
			} catch (IOException ignored) {
			}

			try {
				if (writer != null) writer.close();
			} catch (IOException ignored) {
			}

			if (process != null) {
				process.destroy();
			}
		}

		return fileName;
	}

	/**
	 * Dumps a zip file, containing the current logcat and some internal and external files.
	 * Returns the path string.
	 */
	static String dumpReport() {
		return null;
	}

	static Intent createLogShareIntent(Context ctx, String fileName) {
		if (fileName == null) {
			Toast.makeText(ctx, "Log could not be created!", Toast.LENGTH_LONG).show();
			return null;
		}

		Uri uri = Uri.parse("content://net.itsjustsomedude.swrdg.LogProvider/" + fileName);

		Intent intent = new Intent();
		String title;
		if (fileName.endsWith(".zip")) {
			title = "Share Export";
			intent.setType("application/zip");
		} else {
			title = "Share Log";
			intent.setType("text/plain");
		}

		intent.setAction(Intent.ACTION_SEND);
		intent.putExtra(Intent.EXTRA_STREAM, uri);
		intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);

		return Intent.createChooser(intent, title);
	}
}
