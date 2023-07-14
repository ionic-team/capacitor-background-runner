#!/bin/bash

rm -r ./ios/Plugin/RunnerEngine
cp -R ../ios-engine/Sources/RunnerEngine ./ios/Plugin/

rm -r ./android/android-js-engine
cp -R ../android-engine/android-js-engine ./android/
