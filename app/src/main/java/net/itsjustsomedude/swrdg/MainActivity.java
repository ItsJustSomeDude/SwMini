package net.itsjustsomedude.swrdg;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.media.AudioManager;
import android.os.Build;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.KeyEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.window.OnBackInvokedDispatcher;

import com.touchfoo.swordigo.Debug;
import com.touchfoo.swordigo.GameRenderer;
import com.touchfoo.swordigo.GameView;
import com.touchfoo.swordigo.Native;

import java.lang.ref.WeakReference;
import java.util.Timer;
import java.util.concurrent.CountDownLatch;

public class MainActivity extends Activity implements Runnable {
	private static final String TAG = "MiniGameActivity";
	public PersistentState persistentState;
	AudioManager.OnAudioFocusChangeListener afChangeListener;
	AudioManager audioManager;
	GameView gameView = null;
	boolean textboxInit = false;
	boolean inBackground = false;
	boolean inactive = false;
	RelativeLayout mainViewLayout = null;
	int ticks = 0;
	private Timer myTimer;

	// public IStoreController storeController;
	// public GamesSignIn gamesSignIn;
	// public GameServices gameServices;
	// public Analytics analytics;

	private void TimerMethod() {
		this.runOnUiThread(this);
	}

	void AbandonAudioFocus() {
		this.audioManager.abandonAudioFocus(this.afChangeListener);
	}

	void GetAudioFocus() {
		if (this.audioManager.requestAudioFocus(this.afChangeListener, 3, 1) == 1) {
			Debug.Log("Got audio focus");
		} else {
			Debug.Log("Didn't get audio focus");
		}
	}

	public void StartGettingTextInput(String var1) {
		Debug.Log("StartGettingTextInput");

		EditText tb = this.findViewById(R.id.hiddenEditText);
		if (!textboxInit) {
			Debug.Log("Creating EditText");

			tb.setImeOptions(268435456);
			tb.addTextChangedListener(new TextWatcher() {
				public void afterTextChanged(Editable var1) {
					String var3 = var1.toString();
					Debug.Log("Text changed: " + var3);
					gameView.queueEvent(() -> {
						Native.textInputTextDidChange(var3);
					});
				}

				public void beforeTextChanged(CharSequence var1, int var2, int var3, int var4) {
				}

				public void onTextChanged(CharSequence var1, int var2, int var3, int var4) {
				}
			});
			tb.setOnEditorActionListener((TextView v, int var2, KeyEvent ke) -> {
				if (var2 == EditorInfo.IME_ACTION_DONE) {
					Debug.Log("Text editing done");
					this.gameView.queueEvent(Native::textInputDidFinish);
					return true;
				} else {
					return false;
				}
			});
		}

		if (tb != null) {
			this.mainViewLayout.setVisibility(View.VISIBLE);
			tb.setText(var1);
			tb.setFocusable(true);
			tb.setFocusableInTouchMode(true);
			boolean var2 = tb.requestFocus();
			Debug.Log("Trying to focus edit text: " + var2);
			((InputMethodManager) this.getSystemService(Context.INPUT_METHOD_SERVICE)).showSoftInput(tb, 0);
			tb.setSelection(var1.length());
		}
	}

	public void StopGettingTextInput() {
		EditText tb = this.findViewById(R.id.hiddenEditText);

		Debug.Log("Trying to hide keyboard");
		InputMethodManager inputService = (InputMethodManager) this.getSystemService(Context.INPUT_METHOD_SERVICE);
		if (inputService != null) {
			if (tb != null) {
				inputService.hideSoftInputFromWindow(tb.getWindowToken(), 0);
			}
		}

		this.mainViewLayout.setVisibility(View.GONE);
	}

	void connectStoreControllerWithDelay() {
//		new Handler().postDelayed(() -> {
//			if (this.gameView == null) return;
//			this.gameView.queueEvent(() -> {
//				this.runOnUiThread(() -> {
//					if (this.storeController == null) return;
//					this.storeController.connect();
//				});
//			});
//		}, 200L);
	}

	public boolean isInBackground() {
		return this.inBackground;
	}

	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		Debug.Log("onActivityResult(" + requestCode + "," + resultCode + "," + data);

		// Log share finished, launch game.
		if (requestCode == 1000) {
			this.createGame();
		}

		super.onActivityResult(requestCode, resultCode, data);
	}

	// Deprecation suppressed because it's still used on versions before T.
	@SuppressLint("GestureBackNavigation")
	@SuppressWarnings({"deprecation", "RedundantSuppression"})
	public void onBackPressed() {
		// T+, already handled at the end of onCreate.
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
//			super.onBackPressed();
			return;
		}

		Debug.Log("Back pressed (method)");
		this.handleBack();
	}

	private void handleBack() {
		if (this.gameView != null) {
			this.gameView.queueEvent(Native::handleBackButtonPress);
		}
	}

	private void createGame() {
		LibraryManager.loadMini(this);

		System.loadLibrary("openal-soft");
		System.loadLibrary("swordigo");

		Debug.Log("All libraries loaded.");

		// This needs to be fixed... See TODO in function.
		// NativeBridge.loadLibraries(this);

		NativeBridge.midLoad();

		// TODO: Unsure if this is the best place to do this.
		ModProperties.loadDefaultPropertiesFile(this);

		Debug.Log("onCreate");
		// RemoteConfig.createInstance(this);
		this.persistentState = new PersistentState(this);
		this.audioManager = (AudioManager) this.getSystemService(Context.AUDIO_SERVICE);
		this.afChangeListener = change -> {
		};
		this.getWindow().addFlags(128);

		// 1.4.10
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
			WindowManager.LayoutParams wi = this.getWindow().getAttributes();
			wi.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
			this.getWindow().setAttributes(wi);
		}

		Native.mainActivityRef = new WeakReference<>(this);
		GameActions.mainActivityRef = new WeakReference<>(this);

		Native.setFilesDir(this.getApplicationContext().getFilesDir().toString());
		Native.setCacheDir(this.getApplicationContext().getCacheDir().toString());
		Native.setAssetManager(this.getAssets());

		LNIFunctions.register();
		LuaNativeInterface.init();

		this.setContentView(R.layout.main_activity);
		this.mainViewLayout = this.findViewById(R.id.mainViewLayout);
		// this.analytics = new Analytics(this);
		// this.setupGameServices();
		// this.setupStoreController();
		Native.handleApplicationLaunch();

		NativeBridge.lateLoad();

		this.gameView = new GameView(this);
		this.gameView.setEGLConfigChooser(5, 6, 5, 0, 16, 0);
		this.gameView.setPreserveEGLContextOnPause(true);
		this.gameView.setGameRenderer(new GameRenderer(this));

		this.addContentView(this.gameView, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
		this.connectStoreControllerWithDelay();

		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
			getOnBackInvokedDispatcher()
				.registerOnBackInvokedCallback(OnBackInvokedDispatcher.PRIORITY_DEFAULT, this::handleBack);
		}

		MiniOverlay.mainActivityRef = new WeakReference<>(this);
		MiniOverlay.init(this, this.mainViewLayout);

		ButtonController.init(this, this.mainViewLayout);
//		ButtonController.addButton("Test lol");

//		MiniOverlay.addCheckbox("network", "Enable Networking", "Allow mod to connect to the internet");
//		MiniOverlay.addCheckbox("hello", "Hello, world!", "Subtitle for this preference");
//		MiniOverlay.addCheckbox("hello", "Hello, world!", "Subtitle for this preference");
	}

	protected void onCreate(Bundle bundle) {
		super.onCreate(bundle);

		if (!BuildConfig.DEBUG && CrashHandler.lastRunCrashed(this)) {
			// Show crash dialog BEFORE launching engine, in case of startup crashes.

			String logFile = CrashHandler.dumpLogs(this);

			new AlertDialog.Builder(this)
				.setTitle(R.string.crash_report_title)
				.setMessage(R.string.crash_report_body)
				.setPositiveButton(R.string.report_logs, (DialogInterface dialog, int which) -> {
					Intent intent = CrashHandler.createLogShareIntent(this, logFile);
					startActivityForResult(intent, 1000);
				})
				.setNegativeButton(R.string.report_cancel, (DialogInterface dialog, int which) -> this.createGame())
//				.setNeutralButton(R.string.report_export, (DialogInterface dialog, int which) -> {
//				})
				.setOnCancelListener((DialogInterface dialog) -> this.createGame())
				.show();
			return;
		}

		// Normal startup.
		this.createGame();

		for (String abi : Build.SUPPORTED_ABIS) {
			// Find the first matching supported ABI
			if (abi.equals("arm64-v8a")) {
				break;
			} else if (abi.equals("armeabi-v7a") && !Native.getBooleanFromSP("Mini_32bitAck")) {
				new AlertDialog.Builder(this)
					.setTitle(R.string.title_32bit_warning)
					.setMessage(R.string.body_32bit_warning)
					.setPositiveButton(R.string.btn_32bit_warning, (dialog, which) -> {
						Native.saveBooleanInSP("Mini_32bitAck", true);
					})
					.show();
				break;
			}
		}
	}

	protected void onDestroy() {
		super.onDestroy();
		Debug.Log("onDestroy");
		if (this.gameView != null) {
			CountDownLatch latch = new CountDownLatch(1);
			this.gameView.queueEvent(() -> {
				Debug.Log("handleApplicationQuit");
				Native.handleApplicationQuit();
				latch.countDown();
			});

			try {
				latch.await();
			} catch (InterruptedException ignored) {
			}
		}
	}

	public boolean onKeyDown(int var1, KeyEvent var2) {
		if (var1 == KeyEvent.KEYCODE_MENU) {
			if (this.gameView != null) {
				this.gameView.queueEvent(() -> {
					Debug.Log("onMenuPressed");
					Native.handleMenuButtonPress();
				});
				return true;
			}
		}

		return super.onKeyDown(var1, var2);
	}

	protected void onPause() {
		super.onPause();

		this.inactive = true;
		if (this.gameView != null) {
			this.gameView.queueEvent(() -> {
				Debug.Log("didBecomeInactive");

				if (ModProperties.pauseLostFocus)
					Native.applicationDidBecomeInactive();

				this.runOnUiThread(() -> {
					this.gameView.onPause();
					Debug.Log("GLSurfaceView paused");
				});
			});
		}
	}

	protected void onRestart() {
		super.onRestart();
	}

	protected void onResume() {
		super.onResume();
		if (this.gameView != null)
			this.gameView.setLowProfileUI();

		if (this.inactive) {
			this.inactive = false;

			if (this.gameView == null) return;

			this.gameView.onResume();
			this.gameView.queueEvent(() -> {
				Debug.Log("didBecomeActive");
				Native.applicationDidBecomeActive();

//				this.runOnUiThread(() -> {
//					new Handler().postDelayed(() -> {
//						if (!this.inBackground && this.storeController != null && this.storeController.isConnected()) {
//							this.storeController.queryPurchases();
//						}
//					}, 1000L);
//				});
			});
		}
	}

	protected void onStart() {
		super.onStart();
		if (this.inBackground) {
			this.inBackground = false;
//            this.gameServices.adsHelper.onEnterForeground();
//            if (this.storeController == null) {
//                this.setupStoreController();
//                this.connectStoreControllerWithDelay();
//            } else if (!this.storeController.isConnected()) {
//                this.connectStoreControllerWithDelay();
//            }

			if (this.gameView != null) {
				this.gameView.queueEvent(() -> {
					Debug.Log("didEnterForeground");
					Native.applicationDidEnterForeground();
				});
			}
		}
//        else {
//            this.gameServices.adsHelper.onApplicationStart();
//        }

//        new Handler().postDelayed(() -> {
//               if (!this.inBackground && this.gamesSignIn != null) {
//                    Debug.Log("Calling GamesSignIn.signInSilently from onStart");
//                    this.gamesSignIn.signInSilently();
//                }
//        }, 200L);

		if (this.persistentState == null) return;

		this.persistentState.startMeasuringAppForegroundTime();
//		long var1 = Math.max(this.persistentState.getDelayMillisToReviewFlow(), 3600000L);
//		long var3 = this.persistentState.getTotalForegroundMillisForReviewFlow();
//		Debug.Log(
//			"Delay to review flow: " + var1 / 1000L +
//				" seconds, elapsed foreground time: " + var3 / 1000L +
//				" seconds"
//		);
//		if (var3 >= var1) {
//			this.persistentState.setTotalForegroundMillisForReviewFlow(0L);
//			this.persistentState.setDelayMillisToReviewFlow(var1 * 2L);
//
//			new Handler().postDelayed(() -> {
//				if (!this.inBackground) {
//					this.gameServices.startReviewFlow();
//				}
//			}, 2000L);
//		} else {
//
//			new Handler().postDelayed(() -> {
//				if (!this.inBackground) {
//					this.gameServices.adsHelper.showAppOpenAdIfAppropriate();
//				}
//			}, 10L);
//		}
	}

	protected void onStop() {
		super.onStop();

		if (this.persistentState != null)
			this.persistentState.finishMeasuringAppForegroundTime();

		this.inBackground = true;
//        this.gameServices.adsHelper.onEnterBackground();
		if (this.gameView != null) {
			this.gameView.queueEvent(() -> {
				Debug.Log("didEnterBackground");
				Native.applicationDidEnterBackground();
			});
		}

//        if (this.storeController != null && this.storeController.shouldDisposeWhenGoingToBackground() && !this.storeController.isPurchaseFlowInProgress()) {
//            this.storeController.dispose();
//            this.storeController = null;
//        }
	}

	public void onWindowFocusChanged(boolean var1) {
		super.onWindowFocusChanged(var1);
		if (var1 && this.gameView != null) {
			this.gameView.setImmersiveMode();
		}
	}

	public void run() {
		++this.ticks;
	}

	public void runOnGameThread(Runnable var1) {
		if (this.gameView != null) {
			this.gameView.queueEvent(var1);
		}
	}

	void setupGameServices() {
//		this.gamesSignIn = new GamesSignIn(this, new GamesSignIn.GamesSignInListener(this) {
//			public void onGameServicesSignInCompleted() {
//				Debug.Log("onSignInSucceeded");
//				this.gameServices.loggedIn = true;
//				(new Handler()).postDelayed(() -> {
//					runOnGameThread(() -> {
//						Debug.Log("Calling Native.googleSignInCompleted: true");
//						Native.googleSignInCompleted(true);
//
//					});
//				}, 100L);
//			}
//
//			public void onSignInFailed() {
//				Debug.Log("onSignInFailed");
//				this.gameServices.loggedIn = false;
//				(new Handler()).postDelayed(() -> {
//					runOnGameThread(() -> {
//						Debug.Log("Calling Native.googleSignInCompleted: false");
//						Native.googleSignInCompleted(false);
//					});
//				}, 100L);
//			}
//
//			public void onSignInSucceeded() {
//			}
//		});
//		this.gameServices = new GameServices(this, this.gamesSignIn);
	}

	void setupStoreController() {
//		StoreController var1 = new StoreController(this);
//		this.storeController = var1;
//		var1.addEventListener(new IStoreController.EventListener(this) {
//			public void onConnected() {
//				(new Handler()).postDelayed(() -> {
//					if (this.storeController != null) {
//						this.storeController.queryPurchases();
//					}
//				}, 100L);
//			}
//
//			public void onFinishedRestoringPurchases(int var1) {
//				Debug.Log("MainActivity.onFinishedRestoringPurchases, numberOfPurchasesRestored: " + var1);
//				AdsHelper var4 = this.gameServices.adsHelper;
//				boolean var2;
//				var2 = var1 > 0;
//
//				var4.setAdsDisabledBecausePurchase(var2);
//				runOnGameThread(() -> {
//					Debug.Log("Calling Native.finishedRestoringPurchases");
//					Native.finishedRestoringPurchases();
//				});
//			}
//
//			public void onProductFetchFailed(String identifier, String var2) {
//				runOnGameThread(() -> {
//					Debug.Log("Calling Native.storeProductFetchFailed: " + identifier);
//					Native.storeProductFetchFailed(identifier, this.storeController.fetchErrorMessage());
//				});
//			}
//
//			public void onProductFetched(StoreProduct product) {
//				runOnGameThread(() -> {
//					Debug.Log("Calling Native.storeProductFetched: " + product);
//					Native.storeProductFetched(product.getProductId(), product.getDescription(), product.getPrice());
//				});
//			}
//
//			public void onPurchaseFailed(String productId, String errorMessage) {
//				runOnGameThread(() -> {
//					String var1 = "Calling Native.productPurchaseFailed: " + productId +
//						", " +
//						errorMessage;
//					Debug.Log(var1);
//					Native.productPurchaseFailed(productId, errorMessage);
//				});
//			}
//
//			public void onPurchaseRestored(StorePurchase purchase) {
//				runOnGameThread(() -> {
//					Debug.Log("Calling Native.productPurchased: " + purchase.getProductId());
//					Native.productPurchased(purchase.getProductId());
//				});
//			}
//
//			public void onPurchaseSucceeded(StorePurchase purchase) {
//				gameServices.adsHelper.setAdsDisabledBecausePurchase(true);
//				runOnGameThread(() -> {
//					Debug.Log("Calling Native.productPurchased: " + purchase.getProductId());
//					Native.productPurchased(purchase.getProductId());
//				});
//			}
//
//			public void onStartedRestoringPurchases() {
//				runOnGameThread(() -> {
//					Debug.Log("Calling Native.startedRestoringPurchases");
//					Native.startedRestoringPurchases();
//				});
//			}
//		});
	}
}
