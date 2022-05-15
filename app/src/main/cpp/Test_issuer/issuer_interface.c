//
// Created by benlar on 1/7/22.
//
#include <ibmtss/Unmarshal_fp.h>
#include <ibmtss/tssmarshal.h>
#include <ibmtss/tsscryptoh.h>
#include <stdlib.h>
#include "../cryptoutils.h"
#include "../objecttemplates.h"
#include "../daa/daa_issuer.h"
#include "../defines.h"
#include "../daa_bridge.h"
#include "../daa/daa_client_base.h"
#include "memory.h"
#include "../policy.h"
#include <openssl/err.h>
#include "openssl/evp.h"
#include "openssl/ec.h"

TPM2B_PUBLIC client_ek;
TPM2B_PUBLIC client_wk;
uint8_t issuer_nae[NAME_LEN]; // TODO: This should be made and conveted somewhere

uint8_t correct_policy[DIGEST_SIZE];


TPML_PCR_SELECTION getPCRSelection(int num, ...) {
    TPML_PCR_SELECTION selection;
    selection.count = 1; // We only have one bank (could be multiple ofc)
    selection.pcrSelections[0].hash = HASH_ALG;
    selection.pcrSelections[0].sizeofSelect = 3;    /* hard code 24 PCRs */
    va_list valist;
    va_start(valist, num);

    for (int i = 0; i < 8; i++)
        selection.pcrSelections[0].pcrSelect[i] = 0x00;

    for (int i = 0; i < num; i++) {
        int bit = va_arg(valist, int);
        int bitToSet = ((bit - 1) % 8);
        int byteToSet = (int) ((bit - 1) / 8);
        selection.pcrSelections[0].pcrSelect[byteToSet] |= 1 << bitToSet;
    }

    return selection;
}

void getNameFromPublic(TPMT_PUBLIC *publicKey, unsigned char *nameOut) {
    TPM2B_TEMPLATE marshaled;
    TPMT_HA name;
    uint16_t written;
    uint32_t size;
    uint8_t *buffer;

    name.hashAlg = publicKey->nameAlg;

    written = 0;
    size = sizeof(marshaled.t.buffer);
    buffer = marshaled.t.buffer;

    int rc = TSS_TPMT_PUBLIC_Marshalu(publicKey, &written, &buffer, &size);
    marshaled.t.size = written;

    if (rc == 0) {
        rc = TSS_Hash_Generate(&name, marshaled.t.size, marshaled.t.buffer, 0, NULL);
    } else {
        printf("[-] Error in marshaling key\n");
    }


    nameOut[0] = name.hashAlg >> 8;
    nameOut[1] = name.hashAlg & 0xff;
    memcpy(&nameOut[2], name.digest.tssmax, SHA256_DIGEST_SIZE);

}

void setIssuerName() {
    TPM2B_PUBLIC pk;
    convertEcPemToPublic(&pk, TYPE_SI, TPM_ALG_ECDSA, TPM_ALG_SHA256, TPM_ALG_SHA256,
                         "/home/benlar/Projects/DAA-Bridge_v2/Issuer_keys/public.pem");
    getNameFromPublic(&pk.publicArea, issuer_nae);
}

void loadWalletKey() {
    convertEcPemToPublic(&client_wk, TYPE_SI, TPM_ALG_ECDSA, TPM_ALG_SHA256, TPM_ALG_SHA256,
                         "/home/benlar/Projects/DAA-Bridge_v2/Wallet_keys/public.pem");

}
TPM2B_PUBLIC loadIssuerAuthorizationKey(){

    TPM2B_PUBLIC pk;

    convertEcPemToPublic(&pk,TYPE_SI,TPM_ALG_ECDSA,TPM_ALG_SHA256,TPM_ALG_SHA256,"/home/benlar/Projects/DAA-Bridge_v2/Issuer_keys/public.pem");

    return pk;

}

void onNewSessionFromVCIssuer(TPM2B_PUBLIC *EK, uint8_t* signedNonce, int nonceLen) {
    client_ek = *EK;
    unsigned char cc[4] = {0x00, 0x00, 0x01, 0x6a};
    setIssuerName();

    loadWalletKey();
    // Set correct policy
    memset(correct_policy, 0, DIGEST_SIZE);
    hash_begin(HASH_ALG);
    hash_update(correct_policy, DIGEST_SIZE);
    hash_update(cc, 4);
    hash_update(issuer_nae, NAME_LEN);
    hash_final(correct_policy);

    hash_begin(HASH_ALG);
    hash_update(correct_policy, DIGEST_SIZE);
    hash_final(correct_policy);
    TPM2B_PUBLIC  iss_pk = loadIssuerAuthorizationKey();
    onCreateAttestationKeyCommand(&iss_pk, signedNonce,nonceLen);

}


void get_state_data(TPML_PCR_SELECTION *pcr, uint8_t *state_digest) {

    memset(state_digest, 0, DIGEST_SIZE);
    hash_begin(HASH_ALG);

    hash_update(state_digest, DIGEST_SIZE);
    hash_update(state_digest, DIGEST_SIZE);
    hash_final(state_digest);

}


void get_policy_digest(TPML_PCR_SELECTION *pcr_selection, uint8_t *expected_state_digest, uint8_t *policy_digest) {
    uint8_t wk_name[NAME_LEN];

    // Get name of Wallet Key
    getNameFromPublic(&client_wk.publicArea, wk_name);



    // Clear policyDigest;
    memset(policy_digest, 0, DIGEST_SIZE);

    // Begin by updating with PolicySigned
    updatePolicySigned(wk_name, NAME_LEN, policy_digest);

    // Update with PolicyPCR
    updatePolicyPCR(pcr_selection, expected_state_digest, policy_digest);

}

void cc_to_byteArray_iss(uint16_t cc, uint8_t *out) {
    out[0] = (cc >> 24) & 0xFF;
    out[1] = (cc >> 16) & 0xFF;
    out[2] = (cc >> 8) & 0xFF;
    out[3] = (cc >> 0) & 0xFF;
}

size_t build_join_authorization(char *keyLocation, uint8_t *signature, uint8_t *digestOut, uint8_t * approvedPolicy) {
    EVP_PKEY *key;
    size_t sigLen;
    unsigned int mdLen;
    FILE *pemKeyFile = NULL;
    EVP_MD_CTX *ctx = NULL;
    size_t req = 0;
    uint8_t *signatureLocal;
    ctx = EVP_MD_CTX_create();
    const EVP_MD *md = EVP_get_digestbyname("SHA256");

    // Set digest 0
    memset(digestOut, 0, DIGEST_SIZE);

    TSS_File_Open(&pemKeyFile, keyLocation, "rb");    /* closed @2 */
    if (pemKeyFile == NULL) {
        printf("Pem Keyfile null\n");
    }
    key = PEM_read_PrivateKey(pemKeyFile, NULL, NULL, NULL);

    if (key == NULL) {
        printf("Key is null\n");
    }

    // Calculate Authorized Digest

    hash_begin(HASH_ALG);
    unsigned char pcc[4];
    unsigned char cc[4];
    cc_to_byteArray_iss(TPM_CC_PolicyCommandCode, pcc);
    cc_to_byteArray_iss(TPM_CC_Commit, cc);


    memset(approvedPolicy,0,32);

    hash_update(approvedPolicy,32);
    hash_update(pcc,4);
    hash_update(cc,4);
    hash_final(approvedPolicy);

    // Initialize digest
    if (EVP_DigestInit_ex(ctx, md, NULL) != 1) {
        printf("[-] EVP DigestInit error\n");
    }

    // Initialize signing
    if (EVP_DigestSignInit(ctx, NULL, md, NULL, key) != 1) {
        printf("EVP_DigestSignInit failed, error 0x%lx\n", ERR_get_error());
    }

    // Update with policy digest
    if (EVP_DigestUpdate(ctx, approvedPolicy, DIGEST_SIZE) != 1) {
        printf("[-] EVP SignUpdate error\n");
    }

    // Get signature size
    if (EVP_DigestSignFinal(ctx, NULL, &req) != 1) {
        printf("[-] EVP DigestFinal error\n");
    }
    signatureLocal = OPENSSL_malloc(req);

    // Get signature
    if(signatureLocal == NULL){
        printf("Not able to allocate %zu bytes\n", req);
    }
    if (EVP_DigestSignFinal(ctx, signatureLocal, &req) != 1) {
        printf("[-] EVP DigestFinal (2) error (Issuer)\n");
        printf("DigestFinal 2 failed, error 0x%lx\n", ERR_get_error());

    }

    sigLen = req;
    // Get digest
    EVP_DigestFinal(ctx, digestOut, &mdLen);

    if (ctx) {
        EVP_MD_CTX_destroy(ctx);
        ctx = NULL;
    }
    fclose(pemKeyFile);
    EVP_PKEY_free(key);

    printf("[+] Signed Authorization (Issuer, Join)\n");

    memcpy(signature, signatureLocal, sigLen);
    OPENSSL_free(signatureLocal);

    return sigLen;

}


size_t build_sign_authorization(char *keyLocation, uint8_t *signature, uint8_t *digestOut, TPML_PCR_SELECTION *pcr, uint8_t * approvedPolicy) {
    EVP_PKEY *key;
    size_t sigLen;
    unsigned int mdLen;
    FILE *pemKeyFile = NULL;
    EVP_MD_CTX *ctx = NULL;
    size_t req = 0;
    uint8_t *signatureLocal;
    ctx = EVP_MD_CTX_create();
    const EVP_MD *md = EVP_get_digestbyname("SHA256");
    uint8_t state_digest[SHA256_DIGEST_SIZE];
    uint8_t policy_inner[SHA256_DIGEST_SIZE];

    get_state_data(pcr,state_digest);
    memset(approvedPolicy,0,32);
    get_policy_digest(pcr,state_digest,approvedPolicy);
    memcpy(policy_inner,approvedPolicy,SHA256_DIGEST_SIZE);

    // Set digest 0
    memset(digestOut, 0, DIGEST_SIZE);

    TSS_File_Open(&pemKeyFile, keyLocation, "rb");    /* closed @2 */
    if (pemKeyFile == NULL) {
        printf("Pem Keyfile null\n");
    }
    key = PEM_read_PrivateKey(pemKeyFile, NULL, NULL, NULL);

    if (key == NULL) {
        printf("Key is null\n");
    }

    // Initialize digest
    if (EVP_DigestInit_ex(ctx, md, NULL) != 1) {
        printf("[-] EVP DigestInit error\n");
    }

    // Initialize signing
    if (EVP_DigestSignInit(ctx, NULL, md, NULL, key) != 1) {
        printf("EVP_DigestSignInit failed, error 0x%lx\n", ERR_get_error());
    }

    // Update with policy digest
    if (EVP_DigestUpdate(ctx, policy_inner, DIGEST_SIZE) != 1) {
        printf("[-] EVP SignUpdate error\n");
    }

    // Get signature size
    if (EVP_DigestSignFinal(ctx, NULL, &req) != 1) {
        printf("[-] EVP DigestFinal error\n");
    }
    signatureLocal = OPENSSL_malloc(req);

    // Get signature
    if(signatureLocal == NULL){
        printf("Not able to allocate %zu bytes\n", req);
    }
    if (EVP_DigestSignFinal(ctx, signatureLocal, &req) != 1) {
        printf("[-] EVP DigestFinal (2) error (Issuer)\n");
        printf("DigestFinal 2 failed, error 0x%lx\n", ERR_get_error());

    }

    sigLen = req;
    // Get digest
    EVP_DigestFinal(ctx, digestOut, &mdLen);

    if (ctx) {
        EVP_MD_CTX_destroy(ctx);
        ctx = NULL;
    }
    fclose(pemKeyFile);
    EVP_PKEY_free(key);

    printf("[+] Signed Authorization (Issuer, Sign)\n");

    memcpy(signature, signatureLocal, sigLen);
    OPENSSL_free(signatureLocal);

    return sigLen;


}


void send_issuer_registration(DOOR_ISSUER_REGISTRATION *registrationPackage) {
    // We pretend to be the issuer here
    unsigned char daa_name[NAME_LEN];
    printf("\n[ \t DOOR Issuer Received Key \t]\n");


    AUTHORIZATION signAuth, commitAuth;

    // Step 2: Calculate the name of the provided key
    getNameFromPublic(&registrationPackage->akPub, daa_name);

    // Step 3: Verify the provided key is the one attested


    // Step 5: Verify the policy is PolicyAuthorize(Isssuer)
    if (memcmp(correct_policy, registrationPackage->akPub.authPolicy.t.buffer, DIGEST_SIZE) != 0) {
        printf("[-] Issuer couldn't verify key policy)\n");
        exit(-1);
    } else {
        printf("[+] Provided key has correct policy\n");
    }


    // Check that we have the right policy requirements
    if ((registrationPackage->akPub.objectAttributes.val & TPMA_OBJECT_ADMINWITHPOLICY)  ||
        registrationPackage->akPub.objectAttributes.val & TPMA_OBJECT_USERWITHAUTH) {

        printf("[-] Key Attributes are wrong (Policy)\n");
        exit(-1);
    } else {
        printf("[+] Key Attributes OK\n");
    }


    // Step 6: Build Authorized Policy (Signed WK, PolicyPCR)
    //build_authorization(authorized_digest);

    // Step 7a: Sign the join digest ( for commit )
    printf("\n[ \t DOOR Issuer Authorizes Key to Commit \t]\n");
    commitAuth.sigLen = build_join_authorization("/home/benlar/Projects/DAA-Bridge_v2/Issuer_keys/key.pem",
                                                 commitAuth.signature, commitAuth.digest,commitAuth.approvedPolicy);


    printf("\n[ \t DOOR Issuer Authorizes Key to be use in PCR and Signed Context (By Wallet)\t]\n");
    // Step 7: Sign the authorized digest
    TPML_PCR_SELECTION select = getPCRSelection(2, 1, 12);
    signAuth.sigLen = build_sign_authorization("/home/benlar/Projects/DAA-Bridge_v2/Issuer_keys/key.pem",
                                               signAuth.signature,
                                               signAuth.digest, &select,signAuth.approvedPolicy);


    // Step 8: Execute DAA Join
    DAA_CONTEXT *ctx = new_daa_context();
    ECC_POINT daa;
    memcpy(daa.x_coord, registrationPackage->akPub.unique.ecc.x.t.buffer,
           registrationPackage->akPub.unique.ecc.x.t.size);
    memcpy(daa.y_coord, registrationPackage->akPub.unique.ecc.y.t.buffer,
           registrationPackage->akPub.unique.ecc.y.t.size);
    daa.coord_len = registrationPackage->akPub.unique.ecc.y.t.size;

    printf("\n[ \t Issuer Creating Challenge Credential (DAA Join) \t]\n");


    CHALLENGE_CREDENTIAL c = daa_initiate_join(ctx, &daa, daa_name,
                                               client_ek.publicArea.unique.rsa.t.buffer);
    printf("[*] Bridge Received Challenge Credential\n");
    free_daa_context(ctx);

    // Step 9: Return data to Bridge
    onIssuerChallenge(c, signAuth, commitAuth,select);
}
