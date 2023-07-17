#!/bin/bash

rm -r ./ios/Plugin/RunnerEngine
cp -R ../ios-engine/Sources/RunnerEngine ./ios/Plugin/

cp -R ../android-engine/android-js-engine/build/outputs/aar/android-js-engine-release.aar ./android/src/main/libs
