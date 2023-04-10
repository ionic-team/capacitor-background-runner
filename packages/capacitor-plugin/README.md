# capacitor-plugin

Ionic Enterprise Background Runner

## Install

```bash
npm install capacitor-plugin
npx cap sync
```

## API

<docgen-index>

* [`init()`](#init)
* [`dispatchEvent(...)`](#dispatchevent)
* [Interfaces](#interfaces)

</docgen-index>

<docgen-api>
<!--Update the source file JSDoc comments and rerun docgen to update the docs below-->

### init()

```typescript
init() => any
```

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


### Interfaces


#### DispatchEventOptions

| Prop          | Type                                 |
| ------------- | ------------------------------------ |
| **`event`**   | <code>string</code>                  |
| **`details`** | <code>{ [key: string]: any; }</code> |

</docgen-api>
