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

<code>"geolocation" | "notifications"</code>

</docgen-api>
