# Change Log

## [5.0.0-rc](https://github.com/ionic-team/capacitor-background-runner/compare/4.0.0...5.0.0-rc) (2026-10-30)


### ⚠ BREAKING CHANGES

* **android:** allow offline background execution ([#159](https://github.com/ionic-team/capacitor-background-runner/issues/159))
* align plugin baselines with core ([#162](https://github.com/ionic-team/capacitor-background-runner/issues/162))
* **local-notifications:** plugin now uses local-notifications plugin configuration for smallIcon and sound
* fix typo in CapacitorWatch ([#95](https://github.com/ionic-team/capacitor-background-runner/issues/95))

### Features

* Adding support for Support 16 KB page sizes on Android ([#126](https://github.com/ionic-team/capacitor-background-runner/issues/126)) ([ef998c1](https://github.com/ionic-team/capacitor-background-runner/commit/ef998c124307d8c7ab8e96728797465ffbdaf238))
* Adding Swift Package Manager support ([#137](https://github.com/ionic-team/capacitor-background-runner/issues/137)) ([2579bcc](https://github.com/ionic-team/capacitor-background-runner/commit/2579bcc2cce8722d20aff31873d9658ec6820df4))
* align plugin baselines with core ([#162](https://github.com/ionic-team/capacitor-background-runner/issues/162)) ([f54a32c](https://github.com/ionic-team/capacitor-background-runner/commit/f54a32c0fb163df5dde17673e13817298b47d32a))
* Android JS Engine / Plugin ([#6](https://github.com/ionic-team/capacitor-background-runner/issues/6)) ([1f52918](https://github.com/ionic-team/capacitor-background-runner/commit/1f52918784d91558a3e7798d5449887d7fb5cd32))
* App API ([#99](https://github.com/ionic-team/capacitor-background-runner/issues/99)) ([4cc44dd](https://github.com/ionic-team/capacitor-background-runner/commit/4cc44ddd195cac68b09068e336c32068ddc13166))
* Capacitor 6 support ([#92](https://github.com/ionic-team/capacitor-background-runner/issues/92)) ([3194a7d](https://github.com/ionic-team/capacitor-background-runner/commit/3194a7ddbb51561122f8d737ff298ced4f7fb106))
* Capacitor 7 Support ([#116](https://github.com/ionic-team/capacitor-background-runner/issues/116)) ([0e13e79](https://github.com/ionic-team/capacitor-background-runner/commit/0e13e797565d5c35cd4e2b3449b3df445fc86e52))
* Capacitor 8 support ([#141](https://github.com/ionic-team/capacitor-background-runner/issues/141)) ([f04d668](https://github.com/ionic-team/capacitor-background-runner/commit/f04d66844daee81eefd34bfaf7e88e5a2e11b0ae))
* Capacitor Plugin ([#3](https://github.com/ionic-team/capacitor-background-runner/issues/3)) ([ffac505](https://github.com/ionic-team/capacitor-background-runner/commit/ffac505560c144d2478ed6de49dc7d0c5130b15c))
* **iOS:** Adding Capacitor Web APIs ([#4](https://github.com/ionic-team/capacitor-background-runner/issues/4)) ([7daa335](https://github.com/ionic-team/capacitor-background-runner/commit/7daa3350335989e8caf20c7258074a6dfa5d2cfe))


### Bug Fixes

* **android:** AGP 9.0 no longer supports `proguard-android.txt` ([#153](https://github.com/ionic-team/capacitor-background-runner/issues/153)) ([05fc2ac](https://github.com/ionic-team/capacitor-background-runner/commit/05fc2acc375e8f123e68e3321ba2a29793c26e87))
* **android:** allow offline background execution ([#159](https://github.com/ionic-team/capacitor-background-runner/issues/159)) ([a0c6556](https://github.com/ionic-team/capacitor-background-runner/commit/a0c6556222b804962fa5670de938276679f92a99))
* **android:** enhance action intent handling in Notifications to ensure fallback for invalid actionTypeId ([a484f9f](https://github.com/ionic-team/capacitor-background-runner/commit/a484f9f4450b6ed4f04b643d6ac8aea943fa62d0))
* **android:** enhance notification intent handling to ensure app launch intent is properly set ([a484f9f](https://github.com/ionic-team/capacitor-background-runner/commit/a484f9f4450b6ed4f04b643d6ac8aea943fa62d0))
* **android:** improve notification action intent handling by making intents explicit and updating PendingIntent flags for compatibility ([a484f9f](https://github.com/ionic-team/capacitor-background-runner/commit/a484f9f4450b6ed4f04b643d6ac8aea943fa62d0))
* **android:** JS Engine Reliability Fixes ([#69](https://github.com/ionic-team/capacitor-background-runner/issues/69)) ([18f72b6](https://github.com/ionic-team/capacitor-background-runner/commit/18f72b6c17bb83bb0e9dde3b0b4c1d3db5309b0d))
* **android:** making install_libs script more robust ([#172](https://github.com/ionic-team/capacitor-background-runner/issues/172)) ([a382cf2](https://github.com/ionic-team/capacitor-background-runner/commit/a382cf23f0ef0a71c39ae5f9a5e5ed53aa983b1b))
* **android:** Pass channelId to Notification ([#175](https://github.com/ionic-team/capacitor-background-runner/issues/175)) ([#176](https://github.com/ionic-team/capacitor-background-runner/issues/176)) ([b0f3a65](https://github.com/ionic-team/capacitor-background-runner/commit/b0f3a65247f0f263259657e2b479c439c749fffc))
* **android:** registering WorkRequest on handleOnPause ([#79](https://github.com/ionic-team/capacitor-background-runner/issues/79)) ([d6511b2](https://github.com/ionic-team/capacitor-background-runner/commit/d6511b26ad9fe4b36d94999a5ae478c507e60591))
* **android:** replace kotlinOptions to support Kotlin 2.2.0 ([b4d146d](https://github.com/ionic-team/capacitor-background-runner/commit/b4d146d8a4a4a32b0943e0d658bbafa33fe9930c))
* **android:** resolve the JS engine AAR without a postinstall script ([#177](https://github.com/ionic-team/capacitor-background-runner/issues/177)) ([3b7c73d](https://github.com/ionic-team/capacitor-background-runner/commit/3b7c73d68b2430ed24e48a423bc35f2a4d65883d))
* **android:** smallIcon always using default icon ([#125](https://github.com/ionic-team/capacitor-background-runner/issues/125)) ([2eda1d1](https://github.com/ionic-team/capacitor-background-runner/commit/2eda1d1b74776e5b51a6997a9df70947ea3a73dc))
* **android:** Updating to latest QuickJS library release ([#129](https://github.com/ionic-team/capacitor-background-runner/issues/129)) ([b76dbd4](https://github.com/ionic-team/capacitor-background-runner/commit/b76dbd4dac7ae996fde18170ac2cb459e6ed6c9b))
* **Android:** using fully qualified class name for TimedNotificationPublisher ([68cb610](https://github.com/ionic-team/capacitor-background-runner/commit/68cb610b42940660b7e951d658f300c9623e5969))
* Improved Event Handling ([e512c82](https://github.com/ionic-team/capacitor-background-runner/commit/e512c827056b1efb2be4da995d04b2755d86717e))
* **iOS:** Fixing task repeat mechanism for iOS ([#108](https://github.com/ionic-team/capacitor-background-runner/issues/108)) ([4957271](https://github.com/ionic-team/capacitor-background-runner/commit/495727137b8ae25cd3e67982a4fc48393faf2b18))
* **iOS:** issue where app crash when setting or clearing badge ([#127](https://github.com/ionic-team/capacitor-background-runner/issues/127)) ([8ef1ca0](https://github.com/ionic-team/capacitor-background-runner/commit/8ef1ca00b50cd821a8f7ad662c562875d253dd7f))
* **iOS:** KV, Context fixes ([#83](https://github.com/ionic-team/capacitor-background-runner/issues/83)) ([142e96d](https://github.com/ionic-team/capacitor-background-runner/commit/142e96d6415e4d5f8bcf8b217993cd99e2764659))
* **ios:** remove usage of APIs deprecated in iOS 14 and iOS 15 ([8981234](https://github.com/ionic-team/capacitor-background-runner/commit/8981234bcdad85b582339e61a4d289bc97bd709b))
* **ios:** remove usage of APIs deprecated in iOS 14 and iOS 15 ([6b73444](https://github.com/ionic-team/capacitor-background-runner/commit/6b734447c459a5d76a00d1f0140fcc6e971165f6))
* **iOS:** Removing runloop thread ([#84](https://github.com/ionic-team/capacitor-background-runner/issues/84)) ([11124d2](https://github.com/ionic-team/capacitor-background-runner/commit/11124d27dab552da6e4c3f537607419a702c933d))
* **ios:** resolve requestPermissions for geolocation once the user answers ([#183](https://github.com/ionic-team/capacitor-background-runner/issues/183)) ([7d5f204](https://github.com/ionic-team/capacitor-background-runner/commit/7d5f204e61a3e20612e444b37cb9f6753131d899))
* **ios:** Resolve UNUserNotificationCenterDelegate conflict with other plugins ([#143](https://github.com/ionic-team/capacitor-background-runner/issues/143)) ([55d5b7e](https://github.com/ionic-team/capacitor-background-runner/commit/55d5b7e550ac282befcf6ae23ea6a3cf182098de))
* JS Engine Fixes ([#55](https://github.com/ionic-team/capacitor-background-runner/issues/55)) ([b09f30e](https://github.com/ionic-team/capacitor-background-runner/commit/b09f30e8df57db2a7dbb52ce62cfd88995818f14))
* **local-notifications:** read config from LocalNotifications plugin object ([#170](https://github.com/ionic-team/capacitor-background-runner/issues/170)) ([be5a78e](https://github.com/ionic-team/capacitor-background-runner/commit/be5a78ec232b1389ec17d090f3ef7bd3311dcb44))
* setting JavaVersion to 17 ([#14](https://github.com/ionic-team/capacitor-background-runner/issues/14)) ([5844d0c](https://github.com/ionic-team/capacitor-background-runner/commit/5844d0c814378b9c4ebafe752c297d7110e8c2ba))
* Supporting post install script on Windows ([#12](https://github.com/ionic-team/capacitor-background-runner/issues/12)) ([75b4ed0](https://github.com/ionic-team/capacitor-background-runner/commit/75b4ed0cdb44c4caa506c9969b7fdbbe2e122779))
* Updating to Gradle 8 ([#17](https://github.com/ionic-team/capacitor-background-runner/issues/17)) ([353dbe3](https://github.com/ionic-team/capacitor-background-runner/commit/353dbe331f5ed2344ded407041e001e440b9d0ee))
* using fully qualitifed class name for TimedNotificationPublisher ([c4b3bd8](https://github.com/ionic-team/capacitor-background-runner/commit/c4b3bd8c9f4fb881bfab05b2c95c2e0d7387cee8))


### Code Refactoring

* fix typo in CapacitorWatch ([#95](https://github.com/ionic-team/capacitor-background-runner/issues/95)) ([36d9a61](https://github.com/ionic-team/capacitor-background-runner/commit/36d9a61a9f711825150dcf39be327d2f78208585))

## 3.1.0

### Minor Changes

- 05fc2ac: (Android): Replaces the default proguard file proguard-android.txt with proguard-android-optimize.txt which allows proguard optimizations

## 3.0.0

### Major Changes

- f04d668: Support for Capacitor 8. The Android Min SDK is now 24 and iOS deployment target is 15.

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
