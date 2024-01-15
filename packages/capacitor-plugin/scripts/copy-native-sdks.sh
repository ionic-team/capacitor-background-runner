#!/bin/bash

rm -r ./ios/Plugin/RunnerEngine
cp -R ../ios-engine/Sources/RunnerEngine ./ios/Plugin/

rm -f ./android/src/main/libs/android-js-engine-release.aar
cp -R ../android-js-engine/AndroidJSEngine/build/outputs/aar/AndroidJSEngine-debug.aar ./android/src/main/libs/android-js-engine-release.aar
