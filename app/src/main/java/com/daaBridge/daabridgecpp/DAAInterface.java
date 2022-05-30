package com.daaBridge.daabridgecpp;
import com.daaBridge.androidtpm.AndroidTPM;

public class DAAInterface {
    // Used to load the 'daabridgecpp' library on application startup.
    Thread tpmThread;

    static {
        System.loadLibrary("daabridgecpp");
    }
    public void initiateTPM(){
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
