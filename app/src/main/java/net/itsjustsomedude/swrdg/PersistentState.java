package net.itsjustsomedude.swrdg;

import android.app.Activity;
import android.content.SharedPreferences;

import com.touchfoo.swordigo.Debug;

public class PersistentState {
    Activity activity;
    boolean isMeasuringForegroundTime;
    long measuringForegroundTimeStartTimeMillis;

    public PersistentState(Activity var1) {
        this.activity = var1;
    }

    public void finishMeasuringAppForegroundTime() {
        if (this.isMeasuringForegroundTime) {
            this.isMeasuringForegroundTime = false;
            long var3 = System.currentTimeMillis();
            long var1 = var3 - this.measuringForegroundTimeStartTimeMillis;
            this.measuringForegroundTimeStartTimeMillis = var3;
            this.setTotalForegroundMillisForReviewFlow(this.getTotalForegroundMillisForReviewFlow() + var1);
            this.saveLongInSP("totalForegroundTime", this.getLongFromSP("totalForegroundTime") + var1);
        }

    }

    public long getCurrentTotalForegroundTimeMillis() {
        long var3 = System.currentTimeMillis();
        long var1 = this.measuringForegroundTimeStartTimeMillis;
        return this.getLongFromSP("totalForegroundTime") + (var3 - var1);
    }

    public long getCurrentTotalForegroundTimeMinutes() {
        return this.getCurrentTotalForegroundTimeSeconds() / 60L;
    }

    public long getCurrentTotalForegroundTimeSeconds() {
        return this.getCurrentTotalForegroundTimeMillis() / 1000L;
    }

    public long getDelayMillisToReviewFlow() {
        return this.getLongFromSP("delayToReviewFlow");
    }

    long getLongFromSP(String var1) {
        long var2 = this.activity.getApplicationContext().getSharedPreferences("SwordigoPreferences", 0).getLong(var1, 0L);
        Debug.Log("PersistentState.getLongFromSP: " + var1 +
                ": " +
                var2);
        return var2;
    }

    public long getTotalForegroundMillisForReviewFlow() {
        return this.getLongFromSP("foregroundTimeForReviewFlow");
    }

    void saveLongInSP(String var1, long var2) {
        Debug.Log("PersistentState.saveLongInSP: " + var1 +
                ": " +
                var2);
        SharedPreferences.Editor var5 = this.activity.getApplicationContext().getSharedPreferences("SwordigoPreferences", 0).edit();
        var5.putLong(var1, var2);
        var5.apply();
    }

    public void setDelayMillisToReviewFlow(long var1) {
        this.saveLongInSP("delayToReviewFlow", var1);
    }

    public void setTotalForegroundMillisForReviewFlow(long var1) {
        this.saveLongInSP("foregroundTimeForReviewFlow", var1);
    }

    public void startMeasuringAppForegroundTime() {
        this.isMeasuringForegroundTime = true;
        this.measuringForegroundTimeStartTimeMillis = System.currentTimeMillis();
    }
}
