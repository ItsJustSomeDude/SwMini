package com.touchfoo.swordigo;

import android.app.Activity;
import android.content.res.AssetFileDescriptor;
import android.media.AudioAttributes;
import android.media.MediaPlayer;

import java.io.IOException;

/** We have to suppress this because they are libswordigo functions which java doesn't know about.
 * @noinspection JavaJniMissingFunction unused */
public class MusicPlayer implements MediaPlayer.OnErrorListener, MediaPlayer.OnPreparedListener {
    Activity activity;
    String currentFileName = "";
    String desiredFileName = "";
    boolean looping = false;
    MediaPlayer mediaPlayer;
    boolean preparing = false;
    boolean shouldPlayWhenPrepared = false;
    float volume = 1.0F;

    public MusicPlayer(Activity var1) {
        this.activity = var1;
        this.mediaPlayer = new MediaPlayer();

        this.mediaPlayer.setAudioAttributes(
                new AudioAttributes.Builder()
                        .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
                        .setUsage(AudioAttributes.USAGE_GAME)
                        .build()
        );

        this.mediaPlayer.setOnErrorListener(this);
        this.mediaPlayer.setOnPreparedListener(this);
        this.initMusicPlayer();
    }

    public native void initMusicPlayer();

    public boolean loadFile(String var1) {
        Debug.Log("MusicPlayer loadFile! " + var1);

        this.desiredFileName = var1;
        this.shouldPlayWhenPrepared = false;
        return this.preparing || this.prepareDesiredFile();
    }

    public boolean onError(MediaPlayer var1, int var2, int var3) {
        Debug.Log("MusicPlayer.onError: " + var2 + ", " + var3);
        return false;
    }

    public void onPrepared(MediaPlayer var1) {
        Debug.Log("MusicPlayer onPrepared!");
        this.preparing = false;
        if (this.currentFileName.equals(this.desiredFileName)) {
            if (this.shouldPlayWhenPrepared) {
                this.mediaPlayer.start();
                this.shouldPlayWhenPrepared = false;
            }

            this.updateLoopingSetting();
            this.updateVolumeSetting();
        } else {
            this.prepareDesiredFile();
        }

    }

    public void pause() {
        Debug.Log("MusicPlayer Pause!");
        this.shouldPlayWhenPrepared = false;
        if (!this.preparing && !this.currentFileName.isEmpty()) {
            this.mediaPlayer.pause();
        }

    }

    public void play() {
        Debug.Log("MusicPlayer Play!");
        if (!this.preparing) {
            this.mediaPlayer.start();
        } else {
            this.shouldPlayWhenPrepared = true;
        }

    }

    boolean prepareDesiredFile() {
        if (this.currentFileName.equals(this.desiredFileName)) return true;

        String assetPath = "music/" + desiredFileName.replace('-', '_') + ".mp3";
        Debug.Log("Preparing music asset: " + assetPath);
        try {
            if (!currentFileName.isEmpty()) {
                mediaPlayer.stop();
                mediaPlayer.reset();
            }

            AssetFileDescriptor afd = activity.getAssets().openFd(assetPath);
            mediaPlayer.setDataSource(
                    afd.getFileDescriptor(),
                    afd.getStartOffset(),
                    afd.getLength()
            );
            afd.close();

            preparing = true;
            currentFileName = desiredFileName;
            mediaPlayer.prepareAsync();
            return true;

        } catch (IOException | IllegalArgumentException | SecurityException e) {
            Debug.Log("Asset load failed: ", e);
            return false;
        }
    }

    public void setLooping(boolean var1) {
        // Debug.Log("MusicPlayer setLooping! " + var1);
        this.looping = var1;
        this.updateLoopingSetting();
    }

    public void setVolume(float var1) {
        // Debug.Log("MusicPlayer setVolume!" + var1);

        this.volume = var1;
        this.updateVolumeSetting();
    }

    public void stop() {
        Debug.Log("MusicPlayer stop!");

        this.shouldPlayWhenPrepared = false;
        if (!this.preparing && !this.currentFileName.isEmpty()) {
            this.mediaPlayer.stop();
            this.currentFileName = "";
        }

    }

    void updateLoopingSetting() {
        if (!this.preparing) {
            this.mediaPlayer.setLooping(this.looping);
        }

    }

    void updateVolumeSetting() {
        if (!this.preparing) {
            MediaPlayer var2 = this.mediaPlayer;
            float var1 = this.volume;
            var2.setVolume(var1, var1);
        }

    }
}
