

# Project Breakdown

This project is a sample Android Application running the DAA Core. The structure is as follows.

**AndroidTPM** (*app/androidTPM*)

Contains the IBM Software TPM code, and exposes two functions able to be called from Java. These functions are to start and stop the software TPM. These functions are exposed from androidTPM/src/main/java/com/example/androidtpm/AndroidTPM.java

**jniLibs** (*app/src/main/jniLibs*)

This folder contains the necessary libraries needed for DAA Core to run, compiled for Android. These include AMCl, OpenSSL, and IBM TSS.

**MainActivity** (*daa_core/app/src/main/java/com/example/daabridgecpp/*)

This file is a demonstration of an application executing the native library.

**CPP** (*daa_core/app/src/main/cpp/*)

This folder contains all the C code that is used for the DAA Core to operate, we note the most critical files here.

- native-lib.cpp functions as the interface between Java and C, and contains java-callable functions. 

- demo.c shows an independent demo, where all parties are simulated.

- policy.c/h contains policy-specific funtionality.
- templates.c/h contains templates for creating keys. 

- daa_bridge.c/h contains the primary functions for the DAA Core, and is used by the native lib to execute operations, such as registration and signing. 

- bridge_tpm.c/h contains all the functionality needed to interact with the TPM.

- Test_issuer folder contains the interface needed by the issuer and a sample implementation used for demo purposes. 

- Wallet_keys and Issuer_keys is containing test keys for the demo. 

