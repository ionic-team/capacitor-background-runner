# @capacitor/background-runner

Background Runner provides an event-based standalone JavaScript environment for executing your Javascript code outside of the web view.

## Install

```bash
npm install @capacitor/background-runner
npx cap sync
```

Background Runner has support for various device APIs that require permission from the user prior to use.
## iOS

On iOS you must enable the Background Modes capability. 

![Enable Background Mode Capability in Xcode](https://github.com/ionic-team/capacitor-background-runner/raw/main/docs/enable_background_mode_capability.png)

Once added, you must enable the `Background fetch` and `Background processing` modes at a minimum to enable the ability to register and schedule your background tasks.  

If you will be making use of Geolocation or Push Notifications, enable `Location updates` or `Remote notifications` respectively.

![Configure Background Modes in Xcode](https://github.com/ionic-team/capacitor-background-runner/raw/main/docs/configure_background_modes.png)

After enabling the Background Modes capability, add the following to your app's `AppDelegate.swift`:

```swift
func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?) -> Bool {
    
    // ....
    BackgroundRunnerPlugin.registerBackgroundTask()
    BackgroundRunnerPlugin.handleApplicationDidFinishLaunching(launchOptions: launchOptions)
    // ....

    return true
}
```

To allow the Background Runner to handle remote notifications, add the following:
```swift
func application(_ application: UIApplication, didReceiveRemoteNotification userInfo: [AnyHashable : Any], fetchCompletionHandler completionHandler: @escaping (UIBackgroundFetchResult) -> Void) {
        // ....
        BackgroundRunnerPlugin.dispatchEvent(event: "remoteNotification", eventArgs: userInfo) { result in
            switch result {
            case .success:
                completionHandler(.newData)
            case .failure:
                completionHandler(.failed)
            }
        }
    }
```

### Geolocation
Apple requires privacy descriptions to be specified in `Info.plist` for location information:

- `NSLocationAlwaysUsageDescription` (`Privacy - Location Always Usage Description`)
- `NSLocationWhenInUseUsageDescription` (`Privacy - Location When In Use Usage Description`)

Read about [Configuring `Info.plist`](https://capacitorjs.com/docs/ios/configuration#configuring-infoplist) in the [iOS Guide](https://capacitorjs.com/docs/ios) for more information on setting iOS permissions in Xcode

## Android

### Geolocation
This API requires the following permissions be added to your `AndroidManifest.xml`:

```xml
<!-- Geolocation API -->
<uses-permission android:name="android.permission.ACCESS_COARSE_LOCATION" />
<uses-permission android:name="android.permission.ACCESS_FINE_LOCATION" />
<uses-feature android:name="android.hardware.location.gps" />
```

The first two permissions ask for location data, both fine and coarse, and the last line is optional but necessary if your app _requires_ GPS to function. You may leave it out, though keep in mind that this may mean your app is installed on devices lacking GPS hardware.

### Local Notifications
Android 13 requires a permission check in order to send notifications.  You are required to call `checkPermissions()` and `requestPermissions()` accordingly.

On Android 12 and older it won't show a prompt and will just return as granted.

Starting on Android 12, scheduled notifications won't be exact unless this permission is added to your `AndroidManifest.xml`:

```xml
<uses-permission android:name="android.permission.SCHEDULE_EXACT_ALARM" />
```

Note that even if the permission is present, users can still disable exact notifications from the app settings.

Read about [Setting Permissions](https://capacitorjs.com/docs/android/configuration#setting-permissions) in the [Android Guide](https://capacitorjs.com/docs/android) for more information on setting Android permissions.

## Using Background Runner
Background Runner is an event based JavaScript environment that emits events to a javascript runner file that you designate in your `capacitor.config.ts` file.  If the runner finds a event handler corresponding to incoming event in your runner file, it will execute the event handler, then shutdown once `details.completed()` is called (or if the OS force kills your process).

#### Example Runner JS File

```js
addEventListener("myCustomEvent", (details) => {
  console.log("do something to update the system here");
  details.completed();
});

addEventListener("myCustomEventWithReturnData", (details) => {
  console.log("accepted this data: " + JSON.stringify(details.user));

  const updatedUser = details.user;
  updatedUser.firstName = updatedUser.firstName + " HELLO";
  updatedUser.lastName = updatedUser.lastName + " WORLD";

  details.completed(updatedUser);
});

addEventListener("remoteNotification", (details) => {
  console.log("received silent push notification");

  CapacitorNotifications.schedule([
    {
        id: 100,
        title: "Enterprise Background Runner",
        body: "Received silent push notification",
    },
  ]);

  details.completed();
});

```
Calling `details.completed()` is **required** within every event handler called by the runner.  Failure to do this could result in your runner being killed by the OS if your event is called while the app is in the background.  If the app is in the foreground, async calls to `dispatchEvent` may not resolve.

## Configuring Background Runner
On load, Background Runner will automatically register a background task that will be scheduled and ran once your app is backgrounded.  The settings for this behavior is defined in your `capacitor.config.ts` file:

```ts
const config: CapacitorConfig = {
    plugins: {
        BackgroundRunner: {
            label: "com.example.background.task",
            src: "background.js",
            event: "myCustomEvent",
            repeat: true,
            interval: 2,
            autoStart: false,
        },
    },
}
```


## JavaScript API

Background Runner does not execute your Javascript code in a browser or web view, therefore the typical Web APIs you may be used to may not be available. This includes DOM APIs nor ability to interact with your application's DOM.

Below is a list of the available Web APIs provided in Background Runner:

- [console](https://developer.mozilla.org/en-US/docs/Web/API/console)
    - only `info`, `log`, `warn`, `error` , and `debug` are available
- [TextDecoder](https://developer.mozilla.org/en-US/docs/Web/API/TextDecoder)
    - only `decode` is available
- [TextEncoder](https://developer.mozilla.org/en-US/docs/Web/API/TextEncoder)
    - only `encode` is available
- [addEventListener](https://developer.mozilla.org/en-US/docs/Web/API/EventTarget/addEventListener)
    - Event Listener options and `useCapture` not supported
- [setTimeout](https://developer.mozilla.org/en-US/docs/Web/API/setTimeout)
- [setInterval](https://developer.mozilla.org/en-US/docs/Web/API/setInterval)
- [clearTimeout](https://developer.mozilla.org/en-US/docs/Web/API/clearTimeout)
- [clearInterval](https://developer.mozilla.org/en-US/docs/Web/API/clearInterval)
- [crypto](https://developer.mozilla.org/en-US/docs/Web/API/Crypto)
- [fetch](https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API)
    - Request object not yet supported
    - Only `method`, `headers` and `body` supported in options object

In addition to the standard Web APIs, Background Runner also supports a number of custom APIs that expose relevant mobile device functionality:

- CapacitorKV - a simple string key / value store backed by UserDefaults on iOS and Shared Preferences on Android.
    - `get(key: string): string`
    - `set(key: string, value: string)`
    - `remove(key: string)`
- CapacitorNotifications - basic support for sending local notifications.
    - `schedule()`
- CapacitorDevice - provides information on the device, such as network connectivity and battery status.
    - `getBatteryStatus()`
    - `getNetworkStatus()`
- CapacitorGeolocation - provides access to the device location information
    - `getCurrentPosition()`

## Runner Lifetimes

Currently, the runners are designed for performing periodic bursts of work while your app is in the background, or for executing asynchronous work in a separate thread from your UI while your app is in the foreground.  As a result, runners are not long lived.  State is not maintained between calls to events in the runner.  Each call to `dispatchEvent()` creates a new context in with your runner code is loaded and executed, and once `completed()` is called, the context is destroyed.

## Limitations of Background Tasks

It’s not possible to run persistent, always running background services on mobile operating systems.  Due to the limitations imposed by iOS and Android designed to reduce battery and data consumption, background tasks are constrained with various limitations that you must keep in mind while designing and implementing your background task.

### iOS

- Each invocation of your task has approximately up to 30 seconds of runtime before you must call `completed()` or your task is killed.
- While you can set an interval to define when your task runs after the app is backgrounded, or how often it should run, this is not guaranteed.  iOS will determine when and how often you task will ultimately run, determined in part by how often you app is used.

### Android

- Your task has a maximum of 10 minutes to perform work, but to keep your task cross platform compatible, you should limit your work to 30 seconds at most.
- Repeating background tasks have a minimal interval of at least 15 minutes.  Similar to iOS, any interval you request may not be hit exactly - actual execution time is subject to OS battery optimizations and other heuristics.

## API

<docgen-index>

* [`checkPermissions()`](#checkpermissions)
* [`requestPermissions(...)`](#requestpermissions)
* [`dispatchEvent(...)`](#dispatchevent)
* [Interfaces](#interfaces)
* [Type Aliases](#type-aliases)

</docgen-index>

<docgen-api>
<!--Update the source file JSDoc comments and rerun docgen to update the docs below-->

### checkPermissions()

```typescript
checkPermissions() => any
```

Check permissions for the various Capacitor device APIs.

**Returns:** <code>any</code>

**Since:** 1.0.0

--------------------


### requestPermissions(...)

```typescript
requestPermissions(options: RequestPermissionOptions) => any
```

Request permission to display local notifications.

| Param         | Type                                                                          |
| ------------- | ----------------------------------------------------------------------------- |
| **`options`** | <code><a href="#requestpermissionoptions">RequestPermissionOptions</a></code> |

**Returns:** <code>any</code>

**Since:** 1.0.0

--------------------


### dispatchEvent(...)

```typescript
dispatchEvent(options: DispatchEventOptions) => any
```

Dispatches an event to the configured runner.

| Param         | Type                                                                  |
| ------------- | --------------------------------------------------------------------- |
| **`options`** | <code><a href="#dispatcheventoptions">DispatchEventOptions</a></code> |

**Returns:** <code>any</code>

**Since:** 1.0.0

--------------------


### Interfaces


#### PermissionStatus

| Prop                | Type                                                        |
| ------------------- | ----------------------------------------------------------- |
| **`geolocation`**   | <code><a href="#permissionstate">PermissionState</a></code> |
| **`notifications`** | <code><a href="#permissionstate">PermissionState</a></code> |


#### RequestPermissionOptions

| Prop       | Type            |
| ---------- | --------------- |
| **`apis`** | <code>{}</code> |


#### DispatchEventOptions

| Prop          | Type                                 | Description                                | Since |
| ------------- | ------------------------------------ | ------------------------------------------ | ----- |
| **`label`**   | <code>string</code>                  | The runner label to dispatch the event to  | 1.0.0 |
| **`event`**   | <code>string</code>                  | The name of the registered event listener. | 1.0.0 |
| **`details`** | <code>{ [key: string]: any; }</code> |                                            |       |


### Type Aliases


#### PermissionState

<code>'prompt' | 'prompt-with-rationale' | 'granted' | 'denied'</code>


#### API

<code>'geolocation' | 'notifications'</code>

</docgen-api>
