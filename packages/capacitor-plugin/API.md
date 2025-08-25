
<docgen-api>
<!--Update the source file JSDoc comments and rerun docgen to update the docs below-->

### Interfaces


#### CapacitorDevice

Get information on the device, such as network connectivity and battery status.

| Prop                   | Type                                                             | Description                                    | Since |
| ---------------------- | ---------------------------------------------------------------- | ---------------------------------------------- | ----- |
| **`getBatteryStatus`** | <code>() =&gt; <a href="#batterystatus">BatteryStatus</a></code> | Get the current battery status for the device. | 1.0.0 |
| **`getNetworkStatus`** | <code>() =&gt; <a href="#networkstatus">NetworkStatus</a></code> | Get the current network status for the device. | 1.0.0 |


#### BatteryStatus

| Prop               | Type                 |
| ------------------ | -------------------- |
| **`batteryLevel`** | <code>number</code>  |
| **`isCharging`**   | <code>boolean</code> |


#### NetworkStatus

| Prop                 | Type                 |
| -------------------- | -------------------- |
| **`connected`**      | <code>boolean</code> |
| **`connectionType`** | <code>string</code>  |


#### CapacitorKV

A simple string key / value store backed by UserDefaults on iOS and Shared Preferences on Android.

| Prop         | Type                                                 | Description                            | Since |
| ------------ | ---------------------------------------------------- | -------------------------------------- | ----- |
| **`set`**    | <code>(key: string, value: string) =&gt; void</code> | Set a string value with the given key. | 1.0.0 |
| **`get`**    | <code>(key: string) =&gt; { value: string; }</code>  | Get a string value for the given key.  | 1.0.0 |
| **`remove`** | <code>(key: string) =&gt; void</code>                | Remove a value with the given key.     | 1.0.0 |


#### CapacitorNotifications

Send basic local notifications.

| Prop             | Type                                                                                                | Description                        | Since |
| ---------------- | --------------------------------------------------------------------------------------------------- | ---------------------------------- | ----- |
| **`schedule`**   | <code>(options: {}) =&gt; void</code>                                                               | Schedule a local notification      | 1.0.0 |
| **`setBadge`**   | <code>(options: <a href="#notificationbadgeoptions">NotificationBadgeOptions</a>) =&gt; void</code> | Set the application badge count    | 2.0.0 |
| **`clearBadge`** | <code>() =&gt; void</code>                                                                          | Clears the application badge count | 2.0.0 |


#### NotificationScheduleOptions

| Prop                   | Type                 | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     | Since |
| ---------------------- | -------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ----- |
| **`id`**               | <code>number</code>  | The notification identifier. On Android it's a 32-bit int. So the value should be between -2147483648 and 2147483647 inclusive.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 | 1.0.0 |
| **`title`**            | <code>string</code>  | The title of the notification.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  | 1.0.0 |
| **`body`**             | <code>string</code>  | The body of the notification, shown below the title.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            | 1.0.0 |
| **`scheduleAt`**       | <code>Date</code>    | Date to send this notification.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 | 1.0.0 |
| **`sound`**            | <code>string</code>  | Name of the audio file to play when this notification is displayed. Include the file extension with the filename. On iOS, the file should be in the app bundle. On Android, the file should be in res/raw folder. Recommended format is `.wav` because is supported by both iOS and Android. Only available for iOS and Android &lt; 26. For Android 26+ use channelId of a channel configured with the desired sound. If the sound file is not found, (i.e. empty string or wrong name) the default system notification sound will be used. If not provided, it will produce the default sound on Android and no sound on iOS. | 1.0.0 |
| **`actionTypeId`**     | <code>string</code>  | Associate an action type with this notification.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                | 1.0.0 |
| **`threadIdentifier`** | <code>string</code>  | Used to group multiple notifications. Sets `threadIdentifier` on the [`UNMutableNotificationContent`](https://developer.apple.com/documentation/usernotifications/unmutablenotificationcontent). Only available for iOS.                                                                                                                                                                                                                                                                                                                                                                                                        | 1.0.0 |
| **`summaryArgument`**  | <code>string</code>  | The string this notification adds to the category's summary format string. Sets `summaryArgument` on the [`UNMutableNotificationContent`](https://developer.apple.com/documentation/usernotifications/unmutablenotificationcontent). Only available for iOS.                                                                                                                                                                                                                                                                                                                                                                    | 1.0.0 |
| **`group`**            | <code>string</code>  | Used to group multiple notifications. Calls `setGroup()` on [`NotificationCompat.Builder`](https://developer.android.com/reference/androidx/core/app/NotificationCompat.Builder) with the provided value. Only available for Android.                                                                                                                                                                                                                                                                                                                                                                                           | 1.0.0 |
| **`groupSummary`**     | <code>string</code>  | If true, this notification becomes the summary for a group of notifications. Calls `setGroupSummary()` on [`NotificationCompat.Builder`](https://developer.android.com/reference/androidx/core/app/NotificationCompat.Builder) with the provided value. Only available for Android when using `group`.                                                                                                                                                                                                                                                                                                                          | 1.0.0 |
| **`extra`**            | <code>any</code>     | Set extra data to store within this notification.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               | 1.0.0 |
| **`ongoing`**          | <code>boolean</code> | If true, the notification can't be swiped away. Calls `setOngoing()` on [`NotificationCompat.Builder`](https://developer.android.com/reference/androidx/core/app/NotificationCompat.Builder) with the provided value. Only available for Android.                                                                                                                                                                                                                                                                                                                                                                               | 1.0.0 |
| **`autoCancel`**       | <code>boolean</code> | If true, the notification is canceled when the user clicks on it. Calls `setAutoCancel()` on [`NotificationCompat.Builder`](https://developer.android.com/reference/androidx/core/app/NotificationCompat.Builder) with the provided value. Only available for Android.                                                                                                                                                                                                                                                                                                                                                          | 1.0.0 |
| **`largeBody`**        | <code>string</code>  | Sets a multiline text block for display in a big text notification style.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       | 1.0.0 |
| **`summaryText`**      | <code>string</code>  | Used to set the summary text detail in inbox and big text notification styles. Only available for Android.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      | 1.0.0 |
| **`smallIcon`**        | <code>string</code>  | Set a custom status bar icon. If set, this overrides the `smallIcon` option from Capacitor configuration. Icons should be placed in your app's `res/drawable` folder. The value for this option should be the drawable resource ID, which is the filename without an extension. Only available for Android.                                                                                                                                                                                                                                                                                                                     | 1.0.0 |
| **`largeIcon`**        | <code>string</code>  | Set a large icon for notifications. Icons should be placed in your app's `res/drawable` folder. The value for this option should be the drawable resource ID, which is the filename without an extension. Only available for Android.                                                                                                                                                                                                                                                                                                                                                                                           | 1.0.0 |
| **`channelId`**        | <code>string</code>  | Specifies the channel the notification should be delivered on. If channel with the given name does not exist then the notification will not fire. If not provided, it will use the default channel. Calls `setChannelId()` on [`NotificationCompat.Builder`](https://developer.android.com/reference/androidx/core/app/NotificationCompat.Builder) with the provided value. Only available for Android 26+.                                                                                                                                                                                                                     | 1.0.0 |


#### NotificationBadgeOptions

| Prop                       | Type                | Description                                                                           | Since |
| -------------------------- | ------------------- | ------------------------------------------------------------------------------------- | ----- |
| **`count`**                | <code>number</code> | The number to set on the application badge count.                                     | 2.0.0 |
| **`notificationTitle`**    | <code>string</code> | The **required** title for the associated badge count notification. Only for Android. | 2.0.0 |
| **`notificationSubtitle`** | <code>string</code> | The subtitle for the associated badge count notification. Only for Android.           | 2.0.0 |


#### CapacitorGeolocation

Get access to device location information.

| Prop                     | Type                                                                                   | Description                          | Since |
| ------------------------ | -------------------------------------------------------------------------------------- | ------------------------------------ | ----- |
| **`getCurrentPosition`** | <code>() =&gt; <a href="#getcurrentpositionresult">GetCurrentPositionResult</a></code> | Get the device's last known location | 1.0.0 |


#### GetCurrentPositionResult

| Prop                   | Type                        | Description                                                                                                           | Since |
| ---------------------- | --------------------------- | --------------------------------------------------------------------------------------------------------------------- | ----- |
| **`latitude`**         | <code>number</code>         | Latitude in decimal degrees                                                                                           | 1.0.0 |
| **`longitude`**        | <code>number</code>         | longitude in decimal degrees                                                                                          | 1.0.0 |
| **`accuracy`**         | <code>number</code>         | Accuracy level of the latitude and longitude coordinates in meters                                                    | 1.0.0 |
| **`altitude`**         | <code>number \| null</code> | The altitude the user is at (if available)                                                                            | 1.0.0 |
| **`altitudeAccuracy`** | <code>number \| null</code> | Accuracy level of the altitude coordinate in meters, if available. Available on all iOS versions and on Android 8.0+. | 1.0.0 |
| **`speed`**            | <code>number \| null</code> | The speed the user is traveling (if available)                                                                        | 1.0.0 |
| **`heading`**          | <code>number \| null</code> | The heading the user is facing (if available)                                                                         | 1.0.0 |


#### CapacitorWatch

Interact with a watch paired with this app

sendMessage, transferUserInfo and updateApplicationContext are raw routes to the WCSession delegate methods, but have no effects currently in a <a href="#capacitorwatch">CapacitorWatch</a> Watch application.
They could be used if a native watch app is developed as a companion app to a Capacitor app

| Prop                           | Type                                                                     | Description                                                                                                                                                                               |
| ------------------------------ | ------------------------------------------------------------------------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **`sendMessage`**              | <code>(options: []) =&gt; void</code>                                    | Sends a message to the watch with the sendMessage() WCSession delegate method This has no effect on a <a href="#capacitorwatch">CapacitorWatch</a> watch app                              |
| **`transferUserInfo`**         | <code>(options: []) =&gt; void</code>                                    | Sends information to the watch with the transferUserInfo() WCSession delegate method This has no effect on a <a href="#capacitorwatch">CapacitorWatch</a> watch app                       |
| **`updateApplicationContext`** | <code>(options: []) =&gt; void</code>                                    | Updates the application context on the watch with the updateApplicationContext() WCSession delegate method This has no effect on a <a href="#capacitorwatch">CapacitorWatch</a> watch app |
| **`isReachable`**              | <code>boolean</code>                                                     | Checks to see if the compaion watch is reachable                                                                                                                                          |
| **`updateWatchUI`**            | <code>(options: { watchUI: string; }) =&gt; void</code>                  | Replaces the current UI on the watch with what is specified here.                                                                                                                         |
| **`updateWatchData`**          | <code>(options: { data: { [key: string]: string; }; }) =&gt; void</code> | Updates the data the watch is using to display variables in text and button fields                                                                                                        |


#### CapacitorApp

| Prop           | Type                                                   |
| -------------- | ------------------------------------------------------ |
| **`getState`** | <code>() =&gt; <a href="#appstate">AppState</a></code> |
| **`getInfo`**  | <code>() =&gt; <a href="#appinfo">AppInfo</a></code>   |


#### AppState

| Prop           | Type                 | Description                       | Since |
| -------------- | -------------------- | --------------------------------- | ----- |
| **`isActive`** | <code>boolean</code> | Whether the app is active or not. | 1.0.0 |


#### AppInfo

| Prop          | Type                | Description                                                                                         | Since |
| ------------- | ------------------- | --------------------------------------------------------------------------------------------------- | ----- |
| **`name`**    | <code>string</code> | The name of the app.                                                                                | 1.0.0 |
| **`id`**      | <code>string</code> | The identifier of the app. On iOS it's the Bundle Identifier. On Android it's the Application ID    | 1.0.0 |
| **`build`**   | <code>string</code> | The build version. On iOS it's the CFBundleVersion. On Android it's the versionCode.                | 1.0.0 |
| **`version`** | <code>string</code> | The app version. On iOS it's the CFBundleShortVersionString. On Android it's package's versionName. | 1.0.0 |

</docgen-api>
