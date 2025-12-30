package androidx.core.view;

import android.os.Build;
import android.view.View;
import android.view.WindowInsets;

public class ViewCompat {
	public static WindowInsetsCompat getRootWindowInsets(View v) {
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
			final WindowInsets wi = v.getRootWindowInsets();
			if (wi == null) return null;

			return new WindowInsetsCompat(wi);
		} else {
			return null;
		}
	}

}
