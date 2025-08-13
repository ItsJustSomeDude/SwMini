//package net.itsjustsomedude.swrdg;
//
//import android.app.Activity;
//import android.content.DialogInterface;
//import android.view.ContextThemeWrapper;
//import android.widget.Button;
//import java.util.ArrayList;
//
//public class AlertDialog {
//    Activity activity;
//    ArrayList<ButtonInfo> buttons = new ArrayList<>();
//    android.app.AlertDialog dialog;
//    boolean dialogReady;
//    IAlertDialogListener listener;
//    String message;
//    String title;
//
//    public AlertDialog(Activity var1) {
//        this.activity = var1;
//    }
//
//    public void addButton(String var1, boolean var2) {
//        ButtonInfo var3 = new ButtonInfo(this);
//        var3.label = var1;
//        var3.isCancelButton = var2;
//        this.buttons.add(var3);
//    }
//
//    public void dismiss() {
//        android.app.AlertDialog var1 = this.dialog;
//        if (var1 != null && var1.isShowing()) {
//            this.dialog.dismiss();
//            this.dialog = null;
//        }
//
//    }
//
//    int getThemeStyle() {
//        return 16974393;
//    }
//
//    public void setTitleAndMessage(String var1, String var2) {
//        this.title = var1;
//        this.message = var2;
//    }
//
//    public void show(IAlertDialogListener var1) {
//        this.listener = var1;
//        android.app.AlertDialog.Builder var4 = new android.app.AlertDialog.Builder(new ContextThemeWrapper(this.activity, this.getThemeStyle()));
//        var4.setTitle(this.title);
//        var4.setMessage(this.message);
//        var4.setCancelable(true);
//        var4.setOnCancelListener((dialog) -> {
//            IAlertDialogListener var2 = this.listener;
//            if (var2 != null) {
//                var2.onCanceled();
//            }
//
//        });
//
////                new DialogInterface.OnCancelListener(this, var1) {
////            final AlertDialog this$0;
////            final IAlertDialogListener val$listener;
////
////            {
////                this.this$0 = var1;
////                this.val$listener = var2;
////            }
////
////            public void onCancel(DialogInterface var1) {
////                IAlertDialogListener var2 = this.val$listener;
////                if (var2 != null) {
////                    var2.onCanceled();
////                }
////
////            }
////        });
//
//        for(int var2 = 0; var2 < this.buttons.size(); ++var2) {
//            ButtonInfo var3 = (ButtonInfo)this.buttons.get(var2);
//            if (var3.isCancelButton) {
//                var4.setNegativeButton(var3.label, (DialogInterface.OnClickListener)null);
//            } else {
//                var4.setPositiveButton(var3.label, (DialogInterface.OnClickListener)null);
//            }
//        }
//
//        android.app.AlertDialog var5 = var4.create();
//        this.dialog = var5;
//        this.dialogReady = false;
//        var5.setOnShowListener((dialog) -> {
//            if (!this.dialogReady) {
//                for(int var2 = 0; var2 < this.buttons.size(); ++var2) {
//                    Button var3;
//                    if (((ButtonInfo)this.buttons.get(var2)).isCancelButton) {
//                        var3 = this.dialog.getButton(-2);
//                    } else {
//                        var3 = this.dialog.getButton(-1);
//                    }
//
//                    int finalVar = var2;
//                    var3.setOnClickListener((view) -> {
//                        if (this.listener != null) {
//                            this.listener.onButtonPressed(finalVar);
//                        }
//                    });
//
////                    new View.OnClickListener(this, var2) {
////                        final <undefinedtype> this$1;
////                        final int val$buttonIndex;
////
////                        {
////                            this.this$1 = var1;
////                            this.val$buttonIndex = var2;
////                        }
////
////                        public void onClick(View var1) {
////                            if (this.this$1.val$listener != null) {
////                                this.this$1.val$listener.onButtonPressed(this.val$buttonIndex);
////                            }
////
////                        }
////                    });
//                }
//
//                this.dialogReady = true;
//            }
//
//        });
//
////                new DialogInterface.OnShowListener(this, var1) {
////            final AlertDialog this$0;
////            final IAlertDialogListener val$listener;
////
////            {
////                this.this$0 = var1;
////                this.val$listener = var2;
////            }
////
////            public void onShow(DialogInterface var1) {
////                if (!this.this$0.dialogReady) {
////                    for(int var2 = 0; var2 < this.this$0.buttons.size(); ++var2) {
////                        Button var3;
////                        if (((ButtonInfo)this.this$0.buttons.get(var2)).isCancelButton) {
////                            var3 = this.this$0.dialog.getButton(-2);
////                        } else {
////                            var3 = this.this$0.dialog.getButton(-1);
////                        }
////
////                        var3.setOnClickListener(new View.OnClickListener(this, var2) {
////                            final <undefinedtype> this$1;
////                            final int val$buttonIndex;
////
////                            {
////                                this.this$1 = var1;
////                                this.val$buttonIndex = var2;
////                            }
////
////                            public void onClick(View var1) {
////                                if (this.this$1.val$listener != null) {
////                                    this.this$1.val$listener.onButtonPressed(this.val$buttonIndex);
////                                }
////
////                            }
////                        });
////                    }
////
////                    this.this$0.dialogReady = true;
////                }
////
////            }
////        });
//        this.dialog.show();
//    }
//
//    static class ButtonInfo {
//        public boolean isCancelButton;
//        public String label;
//        final AlertDialog this$0;
//
//        ButtonInfo(AlertDialog var1) {
//            this.this$0 = var1;
//        }
//    }
//
//    public interface IAlertDialogListener {
//        void onButtonPressed(int var1);
//
//        void onCanceled();
//    }
//}