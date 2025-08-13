package com.touchfoo.swordigo;

public class GameTime {
    public static double scaling = 1;

	private static long lastTime = System.nanoTime();
    private static double gameTime = 0.0;

    public static double now() {
        long curr = System.nanoTime();
        long delta = curr - lastTime;
        lastTime = curr;
        gameTime += (delta * 1.0E-9) * scaling;
        return gameTime;
    }
}
