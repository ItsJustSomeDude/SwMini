package net.itsjustsomedude.swrdg;

import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.touchfoo.swordigo.Debug;

import java.lang.ref.WeakReference;

public class MiniOverlay {
	public static WeakReference<MainActivity> mainActivityRef;

	private static boolean overlayVisible = false;

	public static void show() {
		if (overlayVisible) return;
		overlayVisible = true;

		MainActivity mainActivity = mainActivityRef.get();
		if (mainActivity == null) return;

		mainActivity.findViewById(R.id.overlayRoot).setVisibility(View.VISIBLE);
	}

	public static void hide() {
		if (!overlayVisible) return;
		overlayVisible = false;

		MainActivity mainActivity = mainActivityRef.get();
		if (mainActivity == null) return;

		mainActivity.findViewById(R.id.overlayRoot).setVisibility(View.GONE);
	}

	public static void settings() {
		MainActivity mainActivity = mainActivityRef.get();
		if (mainActivity == null) return;

		mainActivity.findViewById(R.id.layoutReadme).setVisibility(View.GONE);
		mainActivity.findViewById(R.id.layoutLogs).setVisibility(View.GONE);
		mainActivity.findViewById(R.id.layoutSettings).setVisibility(View.VISIBLE);
	}

	public static void logs() {
		MainActivity mainActivity = mainActivityRef.get();
		if (mainActivity == null) return;

		mainActivity.findViewById(R.id.layoutReadme).setVisibility(View.GONE);
		mainActivity.findViewById(R.id.layoutLogs).setVisibility(View.VISIBLE);
		mainActivity.findViewById(R.id.layoutSettings).setVisibility(View.GONE);
	}

	public static void readme() {
		MainActivity mainActivity = mainActivityRef.get();
		if (mainActivity == null) return;

		mainActivity.findViewById(R.id.layoutReadme).setVisibility(View.VISIBLE);
		mainActivity.findViewById(R.id.layoutLogs).setVisibility(View.GONE);
		mainActivity.findViewById(R.id.layoutSettings).setVisibility(View.GONE);
	}

	public static void init(MainActivity ctx, ViewGroup view) {
		LayoutInflater inflater = LayoutInflater.from(ctx);
		View floatingLayout = inflater.inflate(R.layout.overlay, view, false);
		FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams(
			(int) (ctx.getResources().getDisplayMetrics().widthPixels * 0.9),
			(int) (ctx.getResources().getDisplayMetrics().heightPixels * 0.9),
			Gravity.CENTER
		);
		floatingLayout.setVisibility(View.GONE);
//        overlayVisible = true;
		ctx.addContentView(floatingLayout, layoutParams);

		TextView poweredBy = ctx.findViewById(R.id.txtPoweredBy);
		TextView modName = ctx.findViewById(R.id.txtModName);

		String version = (ModProperties.overlayModVersion == null || ModProperties.overlayModVersion.isEmpty()) ? null
			: ModProperties.overlayModVersion;

		if (
			ModProperties.overlayModName == null ||
				ModProperties.overlayModName.isEmpty() ||
				ModProperties.overlayModName.equals("SwordigoMini")
		) {
			// No name or Unmodified. Show Mini and bare version number.
			modName.setText("SwordigoMini");
			poweredBy.setText(version);
		} else if (version == null) {
			// Name set, no version. Show "Name" and bare "Powered By"
			modName.setText(ModProperties.overlayModName);
			poweredBy.setText(ctx.getString(R.string.powered_by, "", "SwordigoMini"));
		} else {
			// Name and version set. Show "Name" and "Version - Powered By Mini"
			modName.setText(ModProperties.overlayModName);
			poweredBy.setText(ctx.getString(R.string.powered_by, version + " - ", "SwordigoMini"));
		}

		LinearLayout readmeBtn = ctx.findViewById(R.id.btnReadme);
		readmeBtn.setOnClickListener((v) -> readme());

		ImageButton logsBtn = ctx.findViewById(R.id.btnLogs);
		logsBtn.setOnClickListener((v) -> logs());

		ImageButton settingsBtn = ctx.findViewById(R.id.btnSettings);
		settingsBtn.setOnClickListener((v) -> settings());

		ImageButton closeBtn = ctx.findViewById(R.id.btnClose);
		closeBtn.setOnClickListener((v) -> hide());
	}

	private static void checkboxClicked(View view, String id, boolean newState) {
		Debug.Log("Clicked on pref " + id + ", New state is " + newState);
	}

	public static void addCheckbox(String id, String title, String subtitle) {
		MainActivity mainActivity = mainActivityRef.get();
		if (mainActivity == null) return;

		LinearLayout settingsPanel = mainActivity.findViewById(R.id.layoutSettings);

		LayoutInflater inflater = LayoutInflater.from(mainActivity);
		View layout = inflater.inflate(R.layout.checkbox_setting, settingsPanel, false);
		((TextView) layout.findViewById(R.id.title)).setText(title);
		((TextView) layout.findViewById(R.id.subtitle)).setText(subtitle);

		CheckBox cb = layout.findViewById(R.id.checkbox);

		cb.setOnClickListener((v) -> checkboxClicked(v, id, cb.isChecked()));

		layout.setOnClickListener((view) -> {
			cb.setChecked(!cb.isChecked());
			checkboxClicked(view, id, cb.isChecked());
		});

		settingsPanel.addView(layout);
	}
}
