# Change Log

## 2.3.1

### Patch Changes

- c91c3ce: Set 2.x version to latest-7 tag

## 2.3.0

### Minor Changes

- 2579bcc: iOS: Adding support for Swift Package Manager
- 55d5b7e: Fixes an issue where CapacitorBackgroundRunner overwrites the UNUserNotificationCenter.current().delegate, causing conflicts with other plugins that rely on notification handling

## 2.2.0

### Minor Changes

- 2eda1d1: Fixes an issue where Android Notification small icons were always using the default icon
- b76dbd4: (Android): Updating QuickJS to latest (2025-04-26)
- a484f9f: Adding handling for Android notifications
- ef998c1: Adding support for 16kb page sizes on Android
- 8ef1ca0: Fixes a crash when setting badge on iOS

## 2.1.0

### Minor Changes

- 0e13e79: Adding support to Capacitor 7

## 2.0.0

### Major Changes

- 4cc44dd: Adding a new App API, as well as app badge manipulation methods to the Notifications API
- 36d9a61: Fixing typo in CapacitorWatch interface name
- 3194a7d: Updates to support Capacitor 6

### Minor Changes

- 7adb608: (Android) Adding support for Content-Length in fetch requests
- 4957271: (iOS) Fixing task repeat functionality

## 1.1.0

### Minor Changes

- 142e96d: (iOS) KV: calling `get` on an non-existent key returns null instead of empty object
  (iOS) Fixed an issue within Runner that could potentially cause EXC_BAD_ACCESS crashes
- 18f72b6: More reliability fixes for the JS Engine used in the Background Runner:
  - (Android) Fixes in runtime during background execution
  - (Android) Improvements in native lib handling for testing

  When updating, if you have an existing Android app, be sure to delete the `android-js-engine-release.aar` from `android/src/main/libs`, and add the following to `android/app/build.gradle`:

  ```diff
  ...

  repositories {
      flatDir{
          dirs '../capacitor-cordova-android-plugins/src/main/libs', 'libs'
  +		dirs '../../node_modules/@capacitor/background-runner/android/src/main/libs', 'libs'
      }
  }
  ...

  ```

- d6511b2: (Android): Fixing issue that disabled `appStateChange` events
- b09f30e: Reliability fixes for the JS Engine used in the Background Runner:
  - (Android) Improvements to QuickJS integration
  - (Android / iOS) Improvements in the handling of multiple dispatched events

### Patch Changes

- 174f2fe: Fixing incorrect method name in CapacitorGeolocation
- 11124d2: (iOS): Removing unnecessary run loop thread causing spike in CPU while the app was in the foreground

## [1.0.5](https://github.com/ionic-team/capacitor-background-runner/compare/1.0.4...1.0.5) (2023-08-17)

### Bug Fixes

- using fully qualitifed class name for TimedNotificationPublisher ([c4b3bd8](https://github.com/ionic-team/capacitor-background-runner/commit/c4b3bd8c9f4fb881bfab05b2c95c2e0d7387cee8))

## [1.0.4](https://github.com/ionic-team/capacitor-background-runner/compare/1.0.3...1.0.4) (2023-08-10)

**Note:** Version bump only for package @capacitor/background-runner

## [1.0.3](https://github.com/ionic-team/capacitor-background-runner/compare/1.0.2...1.0.3) (2023-08-10)

**Note:** Version bump only for package @capacitor/background-runner

## [1.0.2](https://github.com/ionic-team/capacitor-background-runner/compare/1.0.1...1.0.2) (2023-08-10)

**Note:** Version bump only for package @capacitor/background-runner

## [1.0.1](https://github.com/ionic-team/capacitor-background-runner/compare/1.0.0...1.0.1) (2023-08-09)

### Bug Fixes

- setting JavaVersion to 17 ([#14](https://github.com/ionic-team/capacitor-background-runner/issues/14)) ([5844d0c](https://github.com/ionic-team/capacitor-background-runner/commit/5844d0c814378b9c4ebafe752c297d7110e8c2ba))
- Supporting post install script on Windows ([#12](https://github.com/ionic-team/capacitor-background-runner/issues/12)) ([75b4ed0](https://github.com/ionic-team/capacitor-background-runner/commit/75b4ed0cdb44c4caa506c9969b7fdbbe2e122779))
- Updating to Gradle 8 ([#17](https://github.com/ionic-team/capacitor-background-runner/issues/17)) ([353dbe3](https://github.com/ionic-team/capacitor-background-runner/commit/353dbe331f5ed2344ded407041e001e440b9d0ee))

# 1.0.0 (2023-07-18)

### Features

- Android JS Engine / Plugin ([#6](https://github.com/ionic-team/capacitor-background-runner/issues/6)) ([1f52918](https://github.com/ionic-team/capacitor-background-runner/commit/1f52918784d91558a3e7798d5449887d7fb5cd32))
- Capacitor Plugin ([#3](https://github.com/ionic-team/capacitor-background-runner/issues/3)) ([ffac505](https://github.com/ionic-team/capacitor-background-runner/commit/ffac505560c144d2478ed6de49dc7d0c5130b15c))
- **iOS:** Adding Capacitor Web APIs ([#4](https://github.com/ionic-team/capacitor-background-runner/issues/4)) ([7daa335](https://github.com/ionic-team/capacitor-background-runner/commit/7daa3350335989e8caf20c7258074a6dfa5d2cfe))

# 1.0.0-rc.1 (2023-07-18)

### Features

- Android JS Engine / Plugin ([#6](https://ionic.io/issues/6)) ([1f52918](https://ionic.io/commits/1f52918784d91558a3e7798d5449887d7fb5cd32))
- Capacitor Plugin ([#3](https://ionic.io/issues/3)) ([ffac505](https://ionic.io/commits/ffac505560c144d2478ed6de49dc7d0c5130b15c))
- **iOS:** Adding Capacitor Web APIs ([#4](https://ionic.io/issues/4)) ([7daa335](https://ionic.io/commits/7daa3350335989e8caf20c7258074a6dfa5d2cfe))
