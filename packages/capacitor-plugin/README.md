# capacitor-plugin

Ionic Enterprise Background Runner

## Install

```bash
npm install capacitor-plugin
npx cap sync
```

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
checkPermissions() => Promise<PermissionStatus>
```

**Returns:** <code>Promise&lt;<a href="#permissionstatus">PermissionStatus</a>&gt;</code>

--------------------


### requestPermissions(...)

```typescript
requestPermissions(options: RequestPermissionOptions) => Promise<PermissionStatus>
```

| Param         | Type                                                                          |
| ------------- | ----------------------------------------------------------------------------- |
| **`options`** | <code><a href="#requestpermissionoptions">RequestPermissionOptions</a></code> |

**Returns:** <code>Promise&lt;<a href="#permissionstatus">PermissionStatus</a>&gt;</code>

--------------------


### dispatchEvent(...)

```typescript
dispatchEvent(options: DispatchEventOptions) => Promise<void>
```

| Param         | Type                                                                  |
| ------------- | --------------------------------------------------------------------- |
| **`options`** | <code><a href="#dispatcheventoptions">DispatchEventOptions</a></code> |

--------------------


### registerBackgroundTask(...)

```typescript
registerBackgroundTask(options: RegisterBackgroundTaskOptions) => Promise<void>
```

| Param         | Type                                                                                    |
| ------------- | --------------------------------------------------------------------------------------- |
| **`options`** | <code><a href="#registerbackgroundtaskoptions">RegisterBackgroundTaskOptions</a></code> |

--------------------


### Interfaces


#### PermissionStatus

| Prop                | Type                                                        |
| ------------------- | ----------------------------------------------------------- |
| **`geolocation`**   | <code><a href="#permissionstate">PermissionState</a></code> |
| **`notifications`** | <code><a href="#permissionstate">PermissionState</a></code> |


#### RequestPermissionOptions

| Prop       | Type               |
| ---------- | ------------------ |
| **`apis`** | <code>API[]</code> |


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
