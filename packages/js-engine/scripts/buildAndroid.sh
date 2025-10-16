#!/bin/bash

set -e

if [ "$#" -ne 3 ]; then
    echo "Builds a Swift static library for Android."
    echo ""
    echo "Usage:   $0 <path_to_ndk> <target_triple> <android_api_level>"
    echo "Example: $0 /Users/swift/Library/Android/sdk/ndk/25.2.9519653 aarch64-linux-android 21"
    exit 1
fi

export ANDROID_NDK_HOME=$1
export TARGET_TRIPLE=$2  # e.g., aarch64-linux-android
export ANDROID_PLATFORM=$3  # e.g., 21
export FINAL_TRIPLE="${TARGET_TRIPLE}${ANDROID_PLATFORM}"

if [ ! -d "$ANDROID_NDK_HOME" ]; then
    echo "❌ Error: NDK path not found at '$ANDROID_NDK_HOME'"
    exit 1
fi

HOST_OS=""
case "$(uname -s)" in
    Linux*)  HOST_OS=linux-x86_64;;
    Darwin*) HOST_OS=darwin-x86_64;;
    *)
        echo "❌ Error: Unsupported host OS. This script works on Linux and macOS."
        exit 1
        ;;
esac

CLANG_VERSION="19"
TOOLCHAIN_PATH="$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/$HOST_OS"
SYSROOT_PATH="$TOOLCHAIN_PATH/sysroot"
LINKER_PATH="$TOOLCHAIN_PATH/lib/clang/$CLANG_VERSION/lib/linux/$TARGET_TRIPLE"

swift build -c release --product JSEngine \
    -Xswiftc -sdk \
    -Xswiftc "$SYSROOT_PATH" \
    -Xswiftc -use-ld=lld \
    -Xlinker -L"$LINKER_PATH"
