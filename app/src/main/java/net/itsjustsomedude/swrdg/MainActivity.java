package net.itsjustsomedude.swrdg;

import android.app.Activity;
import android.content.Context;
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

import com.touchfoo.swordigo.Debug;
import com.touchfoo.swordigo.GameRenderer;
import com.touchfoo.swordigo.GameView;
import com.touchfoo.swordigo.Native;

import java.lang.ref.WeakReference;
import java.util.Timer;
import java.util.concurrent.CountDownLatch;

public class MainActivity extends Activity implements Runnable {
	public PersistentState persistentState;
	AudioManager.OnAudioFocusChangeListener afChangeListener;
	//    public Analytics analytics;
	AudioManager audioManager;
	//    public GameServices gameServices;
	GameView gameView = null;
	//    public GamesSignIn gamesSignIn;
//    EditText hiddenEditText = null;
	boolean textboxInit = false;
	boolean inBackground = false;
	boolean inactive = false;
	RelativeLayout mainViewLayout = null;
	//    public IStoreController storeController;
	int ticks = 0;
	private Timer myTimer;

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
//        (new Handler()).postDelayed(new Runnable(this) {
//            final MainActivity this$0;
//
//            {
//                this.this$0 = var1;
//            }
//
//            public void run() {
//                if (this.this$0.gameView != null) {
//                    this.this$0.gameView.queueEvent(new Runnable(this) {
//                        final <undefinedtype> this$1;
//
//                        {
//                            this.this$1 = var1;
//                        }
//
//                        public void run() {
//                            this.this$1.this$0.runOnUiThread(
//                                    new Runnable(this) {
//                                final <undefinedtype> this$2;
//
//                                {
//                                    this.this$2 = var1;
//                                }
//
//                                public void run() {
//                                    if (this.this$2.this$1.this$0.storeController != null) {
//                                        this.this$2.this$1.this$0.storeController.connect();
//                                    }
//
//                                }
//                            });
//                        }
//                    });
//                }
//
//            }
//        }, 200L);
	}

	public boolean isInBackground() {
		return this.inBackground;
	}

	protected void onActivityResult(int var1, int var2, Intent var3) {
		Debug.Log("onActivityResult(" + var1 + "," + var2 + "," + var3);
		super.onActivityResult(var1, var2, var3);
	}

	public void onBackPressed() {
		if (this.gameView != null) {
			this.gameView.queueEvent(() -> {
				Debug.Log("onBackPressed");
				Native.handleBackButtonPress();
			});
		}
	}

	protected void onCreate(Bundle bundle) {
		super.onCreate(bundle);

		System.loadLibrary("mini");
		System.loadLibrary("openal-soft");
		System.loadLibrary("swordigo");

		Debug.Log("All libraries loaded.");

		NativeBridge.loadLibraries(this);

		NativeBridge.midLoad();
		NativeBridge.setupCStringHooks(this);

		// TODO: Unsure if this is the best place to do this.
		ModProperties.loadDefaultPropertiesFile(this);

		Debug.Log("onCreate");
//        RemoteConfig.createInstance(this);
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

		Native.setFilesDir(this.getApplicationContext().getFilesDir().toString());
		Native.setCacheDir(this.getApplicationContext().getCacheDir().toString());
		Native.setAssetManager(this.getAssets());

		NativeBridge.setupFilePaths(this);

		LNIFunctions.register();
		LuaNativeInterface.init();

		this.setContentView(R.layout.main_activity);
		this.mainViewLayout = this.findViewById(R.id.mainViewLayout);
//        this.analytics = new Analytics(this);
//        this.setupGameServices();
//        this.setupStoreController();
		Native.handleApplicationLaunch();

		NativeBridge.lateLoad();

		this.gameView = new GameView(this);
		this.gameView.setEGLConfigChooser(5, 6, 5, 0, 16, 0);
		this.gameView.setPreserveEGLContextOnPause(true);
		this.gameView.setGameRenderer(new GameRenderer(this));

		this.addContentView(this.gameView, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
		this.connectStoreControllerWithDelay();

		MiniOverlay.mainActivityRef = new WeakReference<>(this);
		MiniOverlay.init(this, this.mainViewLayout);

//		MiniOverlay.addCheckbox("network", "Enable Networking", "Allow mod to connect to the internet");
//		MiniOverlay.addCheckbox("hello", "Hello, world!", "Subtitle for this preference");
//		MiniOverlay.addCheckbox("hello", "Hello, world!", "Subtitle for this preference");
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
		this.gameView.setLowProfileUI();
		if (this.inactive) {
			this.inactive = false;
			if (this.gameView != null) {
				this.gameView.onResume();
				this.gameView.queueEvent(() -> {
					Debug.Log("didBecomeActive");
					Native.applicationDidBecomeActive();

//                    this.runOnUiThread(() -> {
//                        new Handler().postDelayed(() -> {
//                            if(!this.inBackground && this.storeController != null && this.storeController.isConnected()) {
//                                this.storeController.queryPurchases();
//                            }
//                        }, 1000L);
//                    });
				});
			}
		}

	}

	protected void onStart() {
		super.onStart();
		this.persistentState.startMeasuringAppForegroundTime();
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

		long var1 = Math.max(this.persistentState.getDelayMillisToReviewFlow(), 3600000L);
		long var3 = this.persistentState.getTotalForegroundMillisForReviewFlow();
		Debug.Log("Delay to review flow: " + var1 / 1000L +
			" seconds, elapsed foreground time: " +
			var3 / 1000L +
			" seconds");
//        if (var3 >= var1) {
//            this.persistentState.setTotalForegroundMillisForReviewFlow(0L);
//            this.persistentState.setDelayMillisToReviewFlow(var1 * 2L);
//            var8.postDelayed(new Runnable(this) {
//                final MainActivity this$0;
//
//                {
//                    this.this$0 = var1;
//                }
//
//                public void run() {
//                    if (!this.this$0.inBackground) {
//                        this.this$0.gameServices.startReviewFlow();
//                    }
//
//                }
//            }, 2000L);
//        } else {
//            var8.postDelayed(new Runnable(this) {
//                final MainActivity this$0;
//
//                {
//                    this.this$0 = var1;
//                }
//
//                public void run() {
//                    if (!this.this$0.inBackground) {
//                        this.this$0.gameServices.adsHelper.showAppOpenAdIfAppropriate();
//                    }
//
//                }
//            }, 10L);
//        }

	}

	protected void onStop() {
		super.onStop();
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
		if (var1) {
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
//        this.gamesSignIn = new GamesSignIn(this, new GamesSignIn.GamesSignInListener(this) {
//            final MainActivity this$0;
//
//            {
//                this.this$0 = var1;
//            }
//
//            public void onGameServicesSignInCompleted() {
//                Debug.Log("onSignInSucceeded");
//                this.this$0.gameServices.loggedIn = true;
//                (new Handler()).postDelayed(new Runnable(this) {
//                    final <undefinedtype> this$1;
//
//                    {
//                        this.this$1 = var1;
//                    }
//
//                    public void run() {
//                        this.this$1.this$0.runOnGameThread(new Runnable(this) {
//                            final <undefinedtype> this$2;
//
//                            {
//                                this.this$2 = var1;
//                            }
//
//                            public void run() {
//                                Debug.Log("Calling Native.googleSignInCompleted: true");
//                                Native.googleSignInCompleted(true);
//                            }
//                        });
//                    }
//                }, 100L);
//            }
//
//            public void onSignInFailed() {
//                Debug.Log("onSignInFailed");
//                this.this$0.gameServices.loggedIn = false;
//                (new Handler()).postDelayed(new Runnable(this) {
//                    final <undefinedtype> this$1;
//
//                    {
//                        this.this$1 = var1;
//                    }
//
//                    public void run() {
//                        this.this$1.this$0.runOnGameThread(new Runnable(this) {
//                            final <undefinedtype> this$2;
//
//                            {
//                                this.this$2 = var1;
//                            }
//
//                            public void run() {
//                                Debug.Log("Calling Native.googleSignInCompleted: false");
//                                Native.googleSignInCompleted(false);
//                            }
//                        });
//                    }
//                }, 100L);
//            }
//
//            public void onSignInSucceeded() {
//            }
//        });
//        this.gameServices = new GameServices(this, this.gamesSignIn);
	}

	void setupStoreController() {
//        StoreController var1 = new StoreController(this);
//        this.storeController = var1;
//        var1.addEventListener(new IStoreController.EventListener(this) {
//            final MainActivity this$0;
//
//            {
//                this.this$0 = var1;
//            }
//
//            public void onConnected() {
//                (new Handler()).postDelayed(new Runnable(this) {
//                    final <undefinedtype> this$1;
//
//                    {
//                        this.this$1 = var1;
//                    }
//
//                    public void run() {
//                        if (this.this$1.this$0.storeController != null) {
//                            this.this$1.this$0.storeController.queryPurchases();
//                        }
//
//                    }
//                }, 100L);
//            }
//
//            public void onFinishedRestoringPurchases(int var1) {
//                StringBuilder var3 = new StringBuilder("MainActivity.onFinishedRestoringPurchases, numberOfPurchasesRestored: ");
//                var3.append(var1);
//                Debug.Log(var3.toString());
//                AdsHelper var4 = this.this$0.gameServices.adsHelper;
//                boolean var2;
//                if (var1 > 0) {
//                    var2 = true;
//                } else {
//                    var2 = false;
//                }
//
//                var4.setAdsDisabledBecausePurchase(var2);
//                this.this$0.runOnGameThread(new Runnable(this) {
//                    final <undefinedtype> this$1;
//
//                    {
//                        this.this$1 = var1;
//                    }
//
//                    public void run() {
//                        Debug.Log("Calling Native.finishedRestoringPurchases");
//                        Native.finishedRestoringPurchases();
//                    }
//                });
//            }
//
//            public void onProductFetchFailed(String var1, String var2) {
//                this.this$0.runOnGameThread(new Runnable(this, var1) {
//                    final <undefinedtype> this$1;
//                    final String val$identifier;
//
//                    {
//                        this.this$1 = var1;
//                        this.val$identifier = var2;
//                    }
//
//                    public void run() {
//                        StringBuilder var1 = new StringBuilder("Calling Native.storeProductFetchFailed: ");
//                        var1.append(this.val$identifier);
//                        Debug.Log(var1.toString());
//                        Native.storeProductFetchFailed(this.val$identifier, this.this$1.this$0.storeController.fetchErrorMessage());
//                    }
//                });
//            }
//
//            public void onProductFetched(StoreProduct var1) {
//                this.this$0.runOnGameThread(new Runnable(this, var1) {
//                    final <undefinedtype> this$1;
//                    final StoreProduct val$product;
//
//                    {
//                        this.this$1 = var1;
//                        this.val$product = var2;
//                    }
//
//                    public void run() {
//                        StringBuilder var1 = new StringBuilder("Calling Native.storeProductFetched: ");
//                        var1.append(this.val$product);
//                        Debug.Log(var1.toString());
//                        Native.storeProductFetched(this.val$product.getProductId(), this.val$product.getDescription(), this.val$product.getPrice());
//                    }
//                });
//            }
//
//            public void onPurchaseFailed(String var1, String var2) {
//                this.this$0.runOnGameThread(new Runnable(this, var1, var2) {
//                    final <undefinedtype> this$1;
//                    final String val$errorMessage;
//                    final String val$productId;
//
//                    {
//                        this.this$1 = var1;
//                        this.val$productId = var2;
//                        this.val$errorMessage = var3;
//                    }
//
//                    public void run() {
//                        StringBuilder var1 = new StringBuilder("Calling Native.productPurchaseFailed: ");
//                        var1.append(this.val$productId);
//                        var1.append(", ");
//                        var1.append(this.val$errorMessage);
//                        Debug.Log(var1.toString());
//                        Native.productPurchaseFailed(this.val$productId, this.val$errorMessage);
//                    }
//                });
//            }
//
//            public void onPurchaseRestored(StorePurchase var1) {
//                this.this$0.runOnGameThread(new Runnable(this, var1) {
//                    final <undefinedtype> this$1;
//                    final StorePurchase val$purchase;
//
//                    {
//                        this.this$1 = var1;
//                        this.val$purchase = var2;
//                    }
//
//                    public void run() {
//                        StringBuilder var1 = new StringBuilder("Calling Native.productPurchased: ");
//                        var1.append(this.val$purchase.getProductId());
//                        Debug.Log(var1.toString());
//                        Native.productPurchased(this.val$purchase.getProductId());
//                    }
//                });
//            }
//
//            public void onPurchaseSucceeded(StorePurchase var1) {
//                this.this$0.gameServices.adsHelper.setAdsDisabledBecausePurchase(true);
//                this.this$0.runOnGameThread(new Runnable(this, var1) {
//                    final <undefinedtype> this$1;
//                    final StorePurchase val$purchase;
//
//                    {
//                        this.this$1 = var1;
//                        this.val$purchase = var2;
//                    }
//
//                    public void run() {
//                        StringBuilder var1 = new StringBuilder("Calling Native.productPurchased: ");
//                        var1.append(this.val$purchase.getProductId());
//                        Debug.Log(var1.toString());
//                        Native.productPurchased(this.val$purchase.getProductId());
//                    }
//                });
//            }
//
//            public void onStartedRestoringPurchases() {
//                this.this$0.runOnGameThread(new Runnable(this) {
//                    final <undefinedtype> this$1;
//
//                    {
//                        this.this$1 = var1;
//                    }
//
//                    public void run() {
//                        Debug.Log("Calling Native.startedRestoringPurchases");
//                        Native.startedRestoringPurchases();
//                    }
//                });
//            }
//        });
	}
}
