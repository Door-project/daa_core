# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

project("androidtpm")


include_directories(/home/benlar/Projects/OpenSSL/openssl_android/inc/x86_64)
link_directories(/home/benlar/Projects/OpenSSL/openssl_android/lib/x86_64)


add_definitions(-DTPM_POSIX -D_POSIX -D_M_ARM -DTPM_NUVOTON -DUSE_BIT_FIELD_STRUCTURES=NO -DTPM_ENCRYPT_SESSIONS=0)
set(COMPILE_FLAGS "-Wall -Wmissing-declarations -Wnested-externs -Wsign-compare")
set(CMAKE_CXX_FLAGS "${COMPILE_FLAGS}")
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)


# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        androidtpm

        # Sets the library as a shared library.
        SHARED

        ACTCommands.c ACT_spt.c AlgorithmCap.c AlgorithmTests.c AsymmetricCommands.c Attest_spt.c AttestationCommands.c AuditCommands.c Bits.c BnConvert.c BnMath.c BnMemory.c Cancel.c CapabilityCommands.c Clock.c ClockCommands.c CommandAudit.c CommandCodeAttributes.c CommandDispatcher.c ContextCommands.c Context_spt.c CryptDes.c CryptCmac.c CryptEccData.c CryptEccKeyExchange.c CryptEccMain.c CryptEccSignature.c CryptHash.c CryptPrime.c CryptPrimeSieve.c CryptRand.c CryptRsa.c CryptSelfTest.c CryptSmac.c CryptSym.c CryptUtil.c DA.c DebugHelpers.c DictionaryCommands.c DuplicationCommands.c EACommands.c EncryptDecrypt_spt.c Entity.c Entropy.c EphemeralCommands.c ExecCommand.c Global.c Handle.c HashCommands.c Hierarchy.c HierarchyCommands.c IoBuffers.c IntegrityCommands.c Locality.c LocalityPlat.c ManagementCommands.c Manufacture.c Marshal.c MathOnByteBuffers.c Memory.c NVCommands.c NVDynamic.c NVMem.c NVReserved.c NV_spt.c Object.c ObjectCommands.c Object_spt.c PCR.c PP.c PPPlat.c PlatformACT.c PlatformData.c Policy_spt.c Power.c PowerPlat.c PrimeData.c PropertyCap.c RandomCommands.c Response.c ResponseCodeProcessing.c RsaKeyCache.c RunCommand.c Session.c SessionCommands.c SessionProcess.c SigningCommands.c StartupCommands.c SymmetricCommands.c TPMCmdp.c TPMCmds.c TestingCommands.c Ticket.c Time.c TpmAsn1.c TpmFail.c TpmSizeChecks.c TpmToOsslDesSupport.c TpmToOsslMath.c TpmToOsslSupport.c Unique.c Unmarshal.c Vendor_TCG_Test.c X509_ECC.c X509_RSA.c X509_spt.c ntc2lib.c TcpServerPosix.c ntc2.c ACT.h ACT_SetTimeout_fp.h ACT_spt_fp.h ActivateCredential_fp.h AlgorithmCap_fp.h AlgorithmTests_fp.h Attest_spt_fp.h BaseTypes.h Bits_fp.h BnConvert_fp.h BnMath_fp.h BnMemory_fp.h BnValues.h Capabilities.h Certify_fp.h CertifyCreation_fp.h CertifyX509_fp.h ChangeEPS_fp.h ChangePPS_fp.h Clear_fp.h ClearControl_fp.h ClockRateAdjust_fp.h ClockSet_fp.h CommandAttributeData.h CommandAttributes.h CommandAudit_fp.h CommandCodeAttributes_fp.h CommandDispatchData.h CommandDispatcher_fp.h Commands.h Commit_fp.h CompilerDependencies.h Context_spt_fp.h ContextLoad_fp.h ContextSave_fp.h Create_fp.h CreateLoaded_fp.h CreatePrimary_fp.h CryptCmac_fp.h CryptDes_fp.h CryptEcc.h CryptEccKeyExchange_fp.h CryptEccMain_fp.h CryptEccSignature_fp.h CryptHash.h CryptHash_fp.h CryptPrimeSieve_fp.h CryptPrime_fp.h CryptRand.h CryptRand_fp.h CryptRsa.h CryptRsa_fp.h CryptSelfTest_fp.h CryptSmac_fp.h CryptSym.h CryptSym_fp.h CryptTest.h CryptUtil_fp.h DA_fp.h DebugHelpers_fp.h DictionaryAttackLockReset_fp.h DictionaryAttackParameters_fp.h Duplicate_fp.h EC_Ephemeral_fp.h ECC_Parameters_fp.h EccTestData.h ECDH_KeyGen_fp.h ECDH_ZGen_fp.h EncryptDecrypt2_fp.h EncryptDecrypt_fp.h EncryptDecrypt_spt_fp.h Entity_fp.h EventSequenceComplete_fp.h EvictControl_fp.h ExecCommand_fp.h FlushContext_fp.h GetCapability_fp.h GetCommandAuditDigest_fp.h GetRandom_fp.h GetSessionAuditDigest_fp.h GetTestResult_fp.h GetTime_fp.h Global.h GpMacros.h Handle_fp.h Hash_fp.h HashSequenceStart_fp.h HashTestData.h Hierarchy_fp.h HierarchyChangeAuth_fp.h HierarchyControl_fp.h HMAC_fp.h HMAC_Start_fp.h Import_fp.h IncrementalSelfTest_fp.h InternalRoutines.h IoBuffers_fp.h KdfTestData.h LibSupport.h Load_fp.h LoadExternal_fp.h Locality_fp.h TPMInterface.h TPMInterface.c MAC_fp.h MAC_Start_fp.h MakeCredential_fp.h Manufacture_fp.h Marshal_fp.h MathOnByteBuffers_fp.h Memory_fp.h MinMax.h NV.h NV_Certify_fp.h NV_ChangeAuth_fp.h NV_DefineSpace_fp.h NV_Extend_fp.h NV_GlobalWriteLock_fp.h NV_Increment_fp.h NV_Read_fp.h NV_ReadLock_fp.h NV_ReadPublic_fp.h NV_SetBits_fp.h NV_spt_fp.h NV_UndefineSpaceSpecial_fp.h NV_UndefineSpace_fp.h NV_Write_fp.h NV_WriteLock_fp.h NVDynamic_fp.h NVReserved_fp.h Object_fp.h Object_spt_fp.h ObjectChangeAuth_fp.h OIDs.h PCR_Allocate_fp.h PCR_Event_fp.h PCR_Extend_fp.h PCR_fp.h PCR_Read_fp.h PCR_Reset_fp.h PCR_SetAuthPolicy_fp.h PCR_SetAuthValue_fp.h Platform.h Platform_fp.h PlatformACT.h PlatformClock.h PlatformData.h PRNG_TestVectors.h Policy_spt_fp.h PolicyAuthValue_fp.h PolicyAuthorizeNV_fp.h PolicyAuthorize_fp.h PolicyCommandCode_fp.h PolicyCounterTimer_fp.h PolicyCpHash_fp.h PolicyDuplicationSelect_fp.h PolicyGetDigest_fp.h PolicyLocality_fp.h PolicyNameHash_fp.h PolicyNV_fp.h PolicyNvWritten_fp.h PolicyOR_fp.h PolicyPCR_fp.h PolicyPassword_fp.h PolicyPhysicalPresence_fp.h PolicyRestart_fp.h PolicySecret_fp.h PolicySigned_fp.h PolicyTemplate_fp.h PolicyTicket_fp.h Power_fp.h PropertyCap_fp.h PP_Commands_fp.h PP_fp.h PRNG_TestVectors.h Quote_fp.h RSA_Decrypt_fp.h RSA_Encrypt_fp.h ReadClock_fp.h ReadPublic_fp.h ResponseCodeProcessing_fp.h Response_fp.h Rewrap_fp.h RsaKeyCache_fp.h RsaTestData.h SelfTest.h SelfTest_fp.h SequenceComplete_fp.h SequenceUpdate_fp.h Session_fp.h SessionProcess_fp.h SetAlgorithmSet_fp.h SetCommandCodeAuditStatus_fp.h SetPrimaryPolicy_fp.h Shutdown_fp.h Sign_fp.h Simulator_fp.h StartAuthSession_fp.h Startup_fp.h StirRandom_fp.h SupportLibraryFunctionPrototypes_fp.h SymmetricTest.h SymmetricTestData.h TcpServer_fp.h TcpServerPosix_fp.h TestParms_fp.h Ticket_fp.h Time_fp.h Tpm.h TpmAlgorithmDefines.h TpmAsn1.h TpmAsn1_fp.h TPMB.h TpmBuildSwitches.h TpmError.h TpmFail_fp.h TpmProfile.h TpmSizeChecks_fp.h TpmTcpProtocol.h TpmToOsslDesSupport_fp.h TpmToOsslHash.h TpmToOsslMath.h TpmToOsslMath_fp.h TpmToOsslSupport_fp.h TpmToOsslSym.h TpmTypes.h Unmarshal_fp.h Unseal_fp.h VendorString.h Vendor_TCG_Test_fp.h VerifySignature_fp.h X509.h X509_ECC_fp.h X509_RSA_fp.h X509_spt_fp.h ZGen_2Phase_fp.h _TPM_Hash_Data_fp.h _TPM_Hash_End_fp.h _TPM_Hash_Start_fp.h _TPM_Init_fp.h swap.h ntc2lib.h ntc2_fp.h

        # Provides a relative path to your source file(s).
        androidtpm.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.
link_directories(/home/benlar/Projects/OpenSSL/openssl_android/lib/x86_64)
include_directories(/home/benlar/Projects/OpenSSL/openssl_android/inc/x86_64)
find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        androidtpm
        crypto
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})
#add_dependencies(androidtpm ${DEPENDENCY_LIST})

