#!/bin/bash

QT_PATH=$HOME/software/qt/6.10.2
QT_ANDROID=$QT_PATH/android_armv7
QT_HOST=$QT_PATH/gcc_64
NDK=$HOME/Android/Sdk/ndk/27.2.12479018
SDK=$HOME/Android/Sdk
BUILD_DIR=./build/android

mkdir -p $BUILD_DIR

cmake -S . -B $BUILD_DIR \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=armeabi-v7a \
    -DANDROID_PLATFORM=android-28 \
    -DANDROID_NDK=$NDK \
    -DANDROID_SDK_ROOT=$SDK \
    -DANDROID_STL=c++_shared \
    -DANDROID_USE_LEGACY_TOOLCHAIN_FILE=OFF \
    -DCMAKE_FIND_ROOT_PATH=$QT_ANDROID \
    -DCMAKE_PREFIX_PATH=$QT_ANDROID \
    -DQT_HOST_PATH=$QT_HOST \
    -DQT_QMAKE_EXECUTABLE=$QT_ANDROID/bin/qmake \
    -DCMAKE_C_COMPILER=$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/clang \
    -DCMAKE_CXX_COMPILER=$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++ \
    -DCMAKE_GENERATOR=Ninja \
    -DCMAKE_MAKE_PROGRAM=/usr/bin/ninja \
    -DCMAKE_IGNORE_PATH=/usr/lib/cmake \
    -DQT_ADDITIONAL_PACKAGES_PREFIX_PATH=$QT_ANDROID \
    -DCMAKE_PROGRAM_PATH=$QT_ANDROID/bin

cmake --build $BUILD_DIR && ninja -C $BUILD_DIR apk

$SDK/build-tools/36.0.0/apksigner sign --ks android/audioplayer.keystore --ks-key-alias audioplayer --out build/audioplayer-signed.apk ./build/android/android-build/build/outputs/apk/release/android-build-release-unsigned.apk
