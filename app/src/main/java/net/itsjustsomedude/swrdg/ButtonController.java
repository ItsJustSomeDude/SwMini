package net.itsjustsomedude.swrdg;

import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.FrameLayout;

import java.lang.ref.WeakReference;

public class ButtonController {
	private static final String LOG_TAG = "MiniBtnController";
	private static WeakReference<MainActivity> mainActivityRef;
	private static WeakReference<ViewGroup> viewRef;

	public static void init(MainActivity act, ViewGroup view) {
		mainActivityRef = new WeakReference<>(act);
		viewRef = new WeakReference<>(view);
	}

	public static void addButton(CharSequence text) {
		MainActivity ctx = mainActivityRef.get();
		if (ctx == null) return;

		Button btn = new Button(ctx);
		btn.setText(text);
		btn.setVisibility(View.VISIBLE);
		btn.setBackgroundResource(R.drawable.game_button);
//		btn.setPadding(100, 100, 100, 100);

		FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams(
			200,
			125,
			Gravity.BOTTOM
		);

//		viewRef.get().addView(btn, layoutParams);
//		viewRef.get().addContentView(btn, layoutParams);

		ctx.addContentView(btn, layoutParams);

		LayoutInflater.from(ctx);

		Log.d(LOG_TAG, "Added button.");
	}
}
