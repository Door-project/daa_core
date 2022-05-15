//
// Created by benlar on 1/19/22.
//

#include "TPMInterface.h"
#include "TpmBuildSwitches.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#ifdef TPM_WINDOWS
#include <windows.h>
#include <winsock.h>
#endif
#include "TpmTcpProtocol.h"
#include "Manufacture_fp.h"
#include "Platform_fp.h"
#include "Simulator_fp.h"
#ifdef TPM_WINDOWS
#include "TcpServer_fp.h"
#endif
#ifdef TPM_POSIX
#include "TcpServerPosix_fp.h"
#endif
#include "TpmProfile.h"		/* kgold */

#define PURPOSE							\
    "TPM Reference Simulator.\nCopyright Microsoft Corp.\n"
#define DEFAULT_TPM_PORT 2321

#include "TPMInterface.h"
#include <android/log.h>
#define  LOG_TAG    "IBM-TPM"

#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)


void startInternalTPM() {

    printf("MainFromTPMCmds\n");

    LOGD("Starting Software TPM...\n");
    /* command line argument defaults */
    int manufacture = 0;
    int portNum = DEFAULT_TPM_PORT;
    int portNumPlat;
    setvbuf(stdout, 0, _IONBF, 0);      /* output may be going through pipe to log file */

    printf("LIBRARY_COMPATIBILITY_CHECK is %s\n",
           (LIBRARY_COMPATIBILITY_CHECK ? "ON" : "OFF"));
    // Enable NV memory
    _plat__NVEnable(NULL);

    if (manufacture || _plat__NVNeedsManufacture())
    {
        printf("Manufacturing NV state...\n");
        if(TPM_Manufacture(1) != 0)
        {
            // if the manufacture didn't work, then make sure that the NV file doesn't
            // survive. This prevents manufacturing failures from being ignored the
            // next time the code is run.
            _plat__NVDisable(1);
            exit(1);
        }
        // Coverage test - repeated manufacturing attempt
        if(TPM_Manufacture(0) != 1)
        {
            exit(2);
        }
        // Coverage test - re-manufacturing
        TPM_TearDown();
        if(TPM_Manufacture(1) != 0)
        {
            exit(3);
        }
    }
    // Disable NV memory
    _plat__NVDisable(0);
    /* power on the TPM  - kgold MS simulator comes up powered off */
    _rpc__Signal_PowerOn(FALSE);
    _rpc__Signal_NvOn();

    portNumPlat = portNum + 1;
    StartTcpServer(&portNum, &portNumPlat);
}

void stopTPM() {

}
