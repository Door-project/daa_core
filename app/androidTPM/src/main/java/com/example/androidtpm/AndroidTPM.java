package com.example.androidtpm;

public class AndroidTPM {

    // Used to load the 'androidtpm' library on application startup.
    static {
        System.loadLibrary("androidtpm");
    }

    /**
     * A native method that is implemented by the 'androidtpm' native library,
     * which is packaged with this application.
     */
    public native void startTPM();
    public native void endTPM();

}