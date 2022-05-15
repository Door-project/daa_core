#!/bin/bash

NDK_INSTALL_DIR="/tmp/ndk-${USER}"
OPENSSL_DIR="/tmp/openssl-OpenSSL_1_1_1n"

if [ -d "$NDK_INSTALL_DIR/aarch64-linux-android" ]; then
    echo "Toolchain presumed installed"
else
    echo "Creating Toolchain\n"#

#    /home/${USER}/Android/Sdk/ndk/21.4.7075529/build/tools/make-standalone-toolchain.sh --toolchain=x86_64-linux-android
    echo "Decompressing\n"
 #  cd ${NDK_INSTALL_DIR} && tar -xf x86_64-linux-android.tar.bz2 && rm x86_64-linux-android.tar.bz2

fi


if [ -d "$OPENSSL_DIR" ]; then
    echo "OpenSSL presumed to be downloaded"
else
    echo "Downloading OpenSSL\n"
    cd /tmp/ &&  wget https://github.com/openssl/openssl/archive/refs/tags/OpenSSL_1_1_1n.tar.gz --no-check-certificate && tar -xvzf OpenSSL_1_1_1n.tar.gz
fi

export PATH=/tmp/ndk-${USER}/x86_64-linux-android/bin:${PATH}c
export ANDROID_NDK_HOME=/tmp/ndk-${USER}/x86_64-linux-android
echo "Configuring OpenSSL\n"
cd $OPENSSL_DIR && ./Configure android-x86_64
echo "Building OpenSSL\n"
make
