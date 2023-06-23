#!/bin/bash

rm -r ./ios/Plugin/RunnerEngine
cp -R ../ios-engine/Sources/RunnerEngine ./ios/Plugin/

rm -r ./android/android-engine
mkdir -p ./android/android-engine/src

cp -f ../android-engine/build.gradle ./android/android-engine/build.gradle
cp -f ../android-engine/settings.gradle ./android/android-engine/settings.gradle
cp -R ../android-engine/src/ ./android/android-engine/src/
