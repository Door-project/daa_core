package com.example.daabridgecpp;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;

import com.example.androidtpm.AndroidTPM;
import com.example.daabridgecpp.databinding.ActivityMainBinding;

import java.sql.SQLOutput;

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


        tpmThread = new Thread(() -> {
            AndroidTPM TPM = new AndroidTPM();
            TPM.startTPM();
        });

        tpmThread.start();
        System.out.println("STARTING");
        try {
            Thread.sleep(250);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }


        System.out.println("Trying to write Public Key IS/WK");
        String wk_priv = "-----BEGIN EC PRIVATE KEY-----\n" +
                "MHcCAQEEIFwwF26cdqGNrk52CO3Znrv5QfkVH6qJmB/1TdknDm/RoAoGCCqGSM49\n" +
                "AwEHoUQDQgAE7lux68P/xJHfp+xc07KhOmOTB6kXx1sm0+NaSql0xvbX9D/gV982\n" +
                "JG1lwk3qMKlIwYrkBJ7X2+iVBaZPU59/qQ==\n" +
                "-----END EC PRIVATE KEY-----\n";

        String wk = "-----BEGIN PUBLIC KEY-----\n" +
                "MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAE7lux68P/xJHfp+xc07KhOmOTB6kX\n" +
                "x1sm0+NaSql0xvbX9D/gV982JG1lwk3qMKlIwYrkBJ7X2+iVBaZPU59/qQ==\n" +
                "-----END PUBLIC KEY-----\n";

        String ik = "-----BEGIN PUBLIC KEY-----\n" +
                "MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAE4CwPPzL9DS6n2zcDsV1hOadgL25Q\n" +
                "hTF3PuomKkE3/ET4GcPMTkYi8zd2IIUVI/FwY+sWTyHhCxrHkfXKksSAmA==\n" +
                "-----END PUBLIC KEY-----";

        String ik_priv = "-----BEGIN EC PRIVATE KEY-----\n" +
                "MHcCAQEEIF8Cx/viWSyi0gCp/OcbMFJrbKmzO2PwlqA/RNtv9UMZoAoGCCqGSM49\n" +
                "AwEHoUQDQgAE4CwPPzL9DS6n2zcDsV1hOadgL25QhTF3PuomKkE3/ET4GcPMTkYi\n" +
                "8zd2IIUVI/FwY+sWTyHhCxrHkfXKksSAmA==\n" +
                "-----END EC PRIVATE KEY-----\n";

        byte[] walletKeyPriv = wk_priv.getBytes();
        byte[] issuerPk = ik.getBytes();
        byte[] walletPk = wk.getBytes();
        byte[] issuerPriv = ik_priv.getBytes();

        registerWallet_priv(walletKeyPriv);
        registerWalletPK(walletPk);
        registerIssuerPK(issuerPk);
        registerIssuer_priv(issuerPriv);

        // Boot TPM and get Endrosement Key
        // Returns {EK + Nonce}
        String daaInfo = DAAEnable();

        // This is basically what the Wallet does - it signs the nonce
        // Returns nonce_signed
        // IORAM SIGNS HERE
        byte[] signedEnable = prepareEnableResponse(daaInfo); //////////

        // Call CreateEnableResponse: Creates the endorsementkey and returns registration object
        // {AK, EK}
        String issreg = CreateEnableResponse(signedEnable);

        // Send it to the DAA issuer
        String challenge = getIssuerChallenge(issreg);

        // Call back into the core and get a response to the challenge
        String challengeResponse = HandleIssuerChallenge(challenge);

        // Send challenge response back to the issuer and obtain full credential
        String fcre = sendChallengeResponse(challengeResponse);

        // "Enable" the credential
        EnableDAACredential(fcre);


        // Test signature
        byte[] n = startDAASession();

        byte[] signed = walletDoMeASignPlz(n);

        byte[] derp = {0x00, 0x01, 0x03, 0x04};
        DAASign(derp,signed);

    }


    // System Initialization Functions
    public native void initializeTPM(); // Returns Endorsement PK

    public native void registerWalletPK(byte[] pemFile);

    public native void registerWallet_priv(byte[] pemFile);

    public native void registerIssuerPK(byte[] pemFile);

    public native void registerIssuer_priv(byte[] pemFile);

    // DAA functionality for Registration
    public native String DAAEnable(); // Step 1

    public native String CreateEnableResponse(byte[] signedNonce); // Step 2

    public native String HandleIssuerChallenge(String issuerChallenge); // Step 4

    public native void EnableDAACredential(String fullcre); // Step 6

    public native byte[] startDAASession();

    public native String DAASign(byte[] data, byte[] signedNonce);


    // Issuer "interface", basically just converts between JSON and objects: Can be implemented in real issuer
    public native byte[] prepareEnableResponse(String json); // Simulates Wallet signing, and then call CreateEnableResponse with this

    public native String getIssuerChallenge(String json_nonce_and_ek);

    public native String sendChallengeResponse(String cr);

    public native byte[] walletDoMeASignPlz(byte[] nonce); // Simulates Wallet signing, and then call CreateEnableResponse with this


}