# capacitor-plugin

Ionic Enterprise Background Runner

## Install

```bash
npm install capacitor-plugin
npx cap sync
```

## API

<docgen-index>

* [`dispatchEvent(...)`](#dispatchevent)
* [`registerBackgroundTask(...)`](#registerbackgroundtask)
* [Interfaces](#interfaces)

</docgen-index>

<docgen-api>
<!--Update the source file JSDoc comments and rerun docgen to update the docs below-->

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

</docgen-api>
