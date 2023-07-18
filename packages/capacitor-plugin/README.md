# Capacitor Background Runner

## Install

```bash
npm install capacitor-plugin
npx cap sync
```

## JavaScript API

Background Runner does not execute your Javascript code in a browser or web view, therefore the typical Web APIs you may be used to may not be available, including no DOM APIs or ability to interact with your applications DOM.

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

In addition to the standard WebAPI, Background Runner also supports a number of custom APIs that expose relevant mobile device functionality:

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

Currently, the runners are designed for performing periodic bursts of work while your app is in the background, or for executing asynchronous work in a separate thread from your UI while your app is in the foreground.  As a result, runners are not long lived.  State is not maintained between calls to events in the runner.  Each call to `dispatchEvent` creates a new context in with your runner code is loaded and executed, and once `completed()` is called, the context is destroyed.

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
* [`registerBackgroundTask(...)`](#registerbackgroundtask)
* [Interfaces](#interfaces)
* [Type Aliases](#type-aliases)

</docgen-index>

<docgen-api>
<!--Update the source file JSDoc comments and rerun docgen to update the docs below-->

### checkPermissions()

```typescript
checkPermissions() => any
```

**Returns:** <code>any</code>

--------------------


### requestPermissions(...)

```typescript
requestPermissions(options: RequestPermissionOptions) => any
```

| Param         | Type                                                                          |
| ------------- | ----------------------------------------------------------------------------- |
| **`options`** | <code><a href="#requestpermissionoptions">RequestPermissionOptions</a></code> |

**Returns:** <code>any</code>

--------------------


### dispatchEvent(...)

```typescript
dispatchEvent(options: DispatchEventOptions) => any
```

| Param         | Type                                                                  |
| ------------- | --------------------------------------------------------------------- |
| **`options`** | <code><a href="#dispatcheventoptions">DispatchEventOptions</a></code> |

**Returns:** <code>any</code>

--------------------


### registerBackgroundTask(...)

```typescript
registerBackgroundTask(options: RegisterBackgroundTaskOptions) => any
```

| Param         | Type                                                                                    |
| ------------- | --------------------------------------------------------------------------------------- |
| **`options`** | <code><a href="#registerbackgroundtaskoptions">RegisterBackgroundTaskOptions</a></code> |

**Returns:** <code>any</code>

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

| Prop          | Type                                 |
| ------------- | ------------------------------------ |
| **`label`**   | <code>string</code>                  |
| **`event`**   | <code>string</code>                  |
| **`details`** | <code>{ [key: string]: any; }</code> |


#### RegisterBackgroundTaskOptions

| Prop         | Type                                                                      |
| ------------ | ------------------------------------------------------------------------- |
| **`runner`** | <code><a href="#backgroundrunnerconfig">BackgroundRunnerConfig</a></code> |


#### BackgroundRunnerConfig

| Prop            | Type                 |
| --------------- | -------------------- |
| **`label`**     | <code>string</code>  |
| **`src`**       | <code>string</code>  |
| **`event`**     | <code>string</code>  |
| **`repeat`**    | <code>boolean</code> |
| **`interval`**  | <code>number</code>  |
| **`autoStart`** | <code>boolean</code> |


### Type Aliases


#### PermissionState

<code>'prompt' | 'prompt-with-rationale' | 'granted' | 'denied'</code>


#### API

<code>'geolocation' | 'notifications'</code>

</docgen-api>
