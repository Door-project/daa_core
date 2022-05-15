include(ExternalProject)
# To build OpenSSL for android
# Go to Android NDK folder build tools, eg: /home/benlar/Android/Sdk/ndk/21.4.7075529/build/tools
# Make standalone tool-chain: /make-standalone-toolchain.sh --toolchain=aarch64-linux-android
# decompress the toolchain: bzip -d /tmp/ndk-user/aarch64-linux-android.bz
# Extract: tar -vf /tmp/ndk-user/aarch64-linux-android.tar
# export PATH=/tmp/ndk-user/aarch64-linux-android/bin:${PATH}
# export ANDROID_NDK_HOME=/tmp/ndk-user/aarch64-linux-android

externalproject_add(OPENSSL
        URL https://github.com/openssl/openssl/archive/refs/tags/OpenSSL_1_1_1n.tar.gz
        URL_MD5 119de7548858fbf87b995b76b6450149
        BUILD_IN_SOURCE 0
        PREFIX ${CMAKE_BINARY_DIR}
        SOURCE_DIR ${CMAKE_BINARY_DIR}/deps/openssl
        CONFIGURE_COMMAND
        cd <SOURCE_DIR> &&
        ./Configure linux-x86_64
        BUILD_COMMAND
        cd <SOURCE_DIR> &&
        make
        INSTALL_COMMAND ""
        )



set(OPENSSL_INCLUDE_PATH ${CMAKE_BINARY_DIR}/deps/openssl/include)
set(OPENSSL_LIB_PATH ${CMAKE_BINARY_DIR}/deps/openssl)
