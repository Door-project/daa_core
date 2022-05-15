package com.example.daabridgecpp;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;

import com.example.androidtpm.AndroidTPM;
import com.example.daabridgecpp.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'daabridgecpp' library on application startup.
    Thread tpmThread;

    static {
        System.loadLibrary("daabridgecpp");
    }

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());


        // Example of a call to a native method
        TextView tv = binding.sampleText;
        tv.setText(stringFromJNI());

        tpmThread = new Thread(() -> {
            AndroidTPM TPM = new AndroidTPM();
            TPM.startTPM();
        });

        tpmThread.start();
        System.out.println("STARTING");
    }

    /**
     * A native method that is implemented by the 'daabridgecpp' native library,
     * which is packaged with this application.
     */
    public class ECC_POINT{
        byte[] x_cord;
        byte[] y_cord;
    }

    public class DAA_CRED{
        ECC_POINT point[] = new ECC_POINT[4];
        byte[] signature;
    }
    public class DAA_SIGNATURE{
        byte[] sigR;
        byte[] sigS;
        DAA_CRED rcre;

    }
    public native String stringFromJNI();
    public native void registerWalletPK(byte[] pemFile);
    public native byte[] daaIssuerRegistration(byte[] signedReg, int len);
    public native byte[] startDAASession();
    public native DAA_SIGNATURE daaSign(byte[] data, int len, byte[] signedNonce, int signedNonceLen);
    public native DAA_SIGNATURE daaSignWithEvidence(byte[] data, int len, byte[] signedNonce, int signedNonceLen, byte[] evidence, int len);


}