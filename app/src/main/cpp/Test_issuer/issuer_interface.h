//
// Created by benlar on 1/7/22.
//

#ifndef DAA_BRIDGE_V2_ISSUER_INTERFACE_H
#define DAA_BRIDGE_V2_ISSUER_INTERFACE_H

#include "../defines.h"

void send_issuer_registration(DOOR_ISSUER_REGISTRATION* registrationPackage);
void onNewSessionFromVCIssuer(TPM2B_PUBLIC *EK, uint8_t* signedNonce, int nonceLen);
#endif //DAA_BRIDGE_V2_ISSUER_INTERFACE_H
