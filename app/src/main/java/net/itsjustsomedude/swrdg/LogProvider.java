package net.itsjustsomedude.swrdg;

import android.content.ContentProvider;
import android.content.ContentValues;
import android.content.Context;
import android.content.UriMatcher;
import android.database.Cursor;
import android.database.MatrixCursor;
import android.net.Uri;
import android.os.ParcelFileDescriptor;

import androidx.NonNull;
import androidx.Nullable;

import java.io.File;
import java.io.FileNotFoundException;

public class LogProvider extends ContentProvider {
	public static final String AUTHORITY = BuildConfig.APPLICATION_ID + ".LogProvider";
	public static final String LOG_SUBDIR = "export";
	private static final int FILE = 1;

	private final UriMatcher uriMatcher = new UriMatcher(UriMatcher.NO_MATCH);

	@Override
	public boolean onCreate() {
		uriMatcher.addURI(AUTHORITY, "*", FILE);
		return true;
	}

	private File getSharedCacheDir(Context context) {
		// Only expose this subfolder
		return new File(context.getCacheDir(), LOG_SUBDIR);
	}

	private File getFileForUri(Uri uri) {
		Context context = getContext();
		if (context == null) {
			throw new IllegalStateException("No context");
		}

		String fileName = uri.getLastPathSegment();
		if (fileName == null) {
			throw new IllegalArgumentException("Invalid URI");
		}

		File dir = getSharedCacheDir(context);
		File file = new File(dir, fileName);

		// Security check: ensure file is inside shared cache folder
		try {
			String dirPath = dir.getCanonicalPath();
			String filePath = file.getCanonicalPath();
			if (!filePath.startsWith(dirPath)) {
				throw new SecurityException("Path traversal detected");
			}
		} catch (Exception e) {
			throw new SecurityException("Invalid file path", e);
		}

		return file;
	}

	@Nullable
	@Override
	public ParcelFileDescriptor openFile(@NonNull Uri uri, @NonNull String mode)
		throws FileNotFoundException {

		if (uriMatcher.match(uri) != FILE) {
			throw new FileNotFoundException("Unknown URI: " + uri);
		}

		File file = getFileForUri(uri);

		if (!file.exists()) {
			throw new FileNotFoundException("File not found: " + uri);
		}

		// Read-only access
		return ParcelFileDescriptor.open(file, ParcelFileDescriptor.MODE_READ_ONLY);
	}

	// --- Not implemented (not required for simple file sharing) ---

	@Nullable
	@Override
	public Cursor query(@NonNull Uri uri, String[] projection, String selection,
						String[] selectionArgs, String sortOrder) {
		if (uriMatcher.match(uri) != FILE) {
			throw new IllegalArgumentException("Unknown URI: " + uri);
		}

		File file = getFileForUri(uri);

		if (!file.exists()) {
			return null; // or throw FileNotFoundException wrapped as runtime
		}

		// Default columns expected by most clients
		String[] defaultProjection = new String[]{
			android.provider.OpenableColumns.DISPLAY_NAME,
			android.provider.OpenableColumns.SIZE
		};

		String[] cols = projection != null ? projection : defaultProjection;

		Object[] values = new Object[cols.length];

		for (int i = 0; i < cols.length; i++) {
			switch (cols[i]) {
				case android.provider.OpenableColumns.DISPLAY_NAME:
					values[i] = file.getName();
					break;

				case android.provider.OpenableColumns.SIZE:
					values[i] = file.length();
					break;

				default:
					values[i] = null;
			}
		}

		MatrixCursor cursor = new MatrixCursor(cols, 1);
		cursor.addRow(values);
		return cursor;
	}

	@Nullable
	@Override
	public String getType(@NonNull Uri uri) {
		return null;
	}

	@Nullable
	@Override
	public Uri insert(@NonNull Uri uri, ContentValues values) {
		throw new UnsupportedOperationException();
	}

	@Override
	public int delete(@NonNull Uri uri, String selection, String[] selectionArgs) {
		throw new UnsupportedOperationException();
	}

	@Override
	public int update(@NonNull Uri uri, ContentValues values, String selection,
					  String[] selectionArgs) {
		throw new UnsupportedOperationException();
	}
}
