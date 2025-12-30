package androidx.core.view;

import android.view.WindowInsets;

public class WindowInsetsCompat {
	final WindowInsets insets;

	public WindowInsetsCompat(WindowInsets insets) {
		this.insets = insets;
	}
	
	public WindowInsets toWindowInsets() {
		return this.insets;
	}
}
