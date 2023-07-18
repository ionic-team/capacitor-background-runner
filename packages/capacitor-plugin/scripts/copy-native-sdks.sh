#!/bin/bash

rm -r ./ios/Plugin/RunnerEngine
cp -R ../ios-engine/Sources/RunnerEngine ./ios/Plugin/

rm -f ./android/src/main/libs/android-js-engine-release.aar
cp -R ../android-engine/android-js-engine/build/outputs/aar/android-js-engine-release.aar ./android/src/main/libs
