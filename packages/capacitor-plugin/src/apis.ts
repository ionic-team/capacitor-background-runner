export interface GetCurrentPositionResult {
  /**
   * Latitude in decimal degrees
   *
   * @since 1.0.0
   */
  latitude: number;
  /**
   * longitude in decimal degrees
   *
   * @since 1.0.0
   */
  longitude: number;
  /**
   * Accuracy level of the latitude and longitude coordinates in meters
   *
   * @since 1.0.0
   */
  accuracy: number;
  /**
   * The altitude the user is at (if available)
   *
   * @since 1.0.0
   */
  altitude: number | null;
  /**
   * Accuracy level of the altitude coordinate in meters, if available.
   *
   * Available on all iOS versions and on Android 8.0+.
   *
   * @since 1.0.0
   */
  altitudeAccuracy?: number | null | undefined;
  /**
   * The speed the user is traveling (if available)
   *
   * @since 1.0.0
   */
  speed: number | null;
  /**
   * The heading the user is facing (if available)
   *
   * @since 1.0.0
   */
  heading: number | null;
}

export interface BatteryStatus {
  batteryLevel: number;
  isCharging: boolean;
}

export interface NetworkStatus {
  connected: boolean;
  connectionType: string;
}

export interface NotificationScheduleOptions {
  /**
   * The notification identifier.
   * On Android it's a 32-bit int.
   * So the value should be between -2147483648 and 2147483647 inclusive.
   *
   * @since 1.0.0
   */
  id: number;
  /**
   * The title of the notification.
   *
   * @since 1.0.0
   */
  title: string;
  /**
   * The body of the notification, shown below the title.
   *
   * @since 1.0.0
   */
  body: string;
  /**
   * Date to send this notification.
   *
   * @since 1.0.0
   */
  scheduleAt: Date;
  /**
   * Name of the audio file to play when this notification is displayed.
   *
   * Include the file extension with the filename.
   *
   * On iOS, the file should be in the app bundle.
   * On Android, the file should be in res/raw folder.
   *
   * Recommended format is `.wav` because is supported by both iOS and Android.
   *
   * Only available for iOS and Android < 26.
   * For Android 26+ use channelId of a channel configured with the desired sound.
   *
   * If the sound file is not found, (i.e. empty string or wrong name)
   * the default system notification sound will be used.
   * If not provided, it will produce the default sound on Android and no sound on iOS.
   *
   * @since 1.0.0
   */
  sound?: string;
  /**
   * Associate an action type with this notification.
   *
   * @since 1.0.0
   */
  actionTypeId?: string;
  /**
   * Used to group multiple notifications.
   *
   * Sets `threadIdentifier` on the
   * [`UNMutableNotificationContent`](https://developer.apple.com/documentation/usernotifications/unmutablenotificationcontent).
   *
   * Only available for iOS.
   *
   * @since 1.0.0
   */
  threadIdentifier?: string;
  /**
   * The string this notification adds to the category's summary format string.
   *
   * Sets `summaryArgument` on the
   * [`UNMutableNotificationContent`](https://developer.apple.com/documentation/usernotifications/unmutablenotificationcontent).
   *
   * Only available for iOS.
   *
   * @since 1.0.0
   */
  summaryArgument?: string;
  /**
   * Used to group multiple notifications.
   *
   * Calls `setGroup()` on
   * [`NotificationCompat.Builder`](https://developer.android.com/reference/androidx/core/app/NotificationCompat.Builder)
   * with the provided value.
   *
   * Only available for Android.
   *
   * @since 1.0.0
   */
  group?: string;
  /**
   * If true, this notification becomes the summary for a group of
   * notifications.
   *
   * Calls `setGroupSummary()` on
   * [`NotificationCompat.Builder`](https://developer.android.com/reference/androidx/core/app/NotificationCompat.Builder)
   * with the provided value.
   *
   * Only available for Android when using `group`.
   *
   * @since 1.0.0
   */
  groupSummary?: string;
  /**
   * Set extra data to store within this notification.
   *
   * @since 1.0.0
   */
  extra?: any;
  /**
   * If true, the notification can't be swiped away.
   *
   * Calls `setOngoing()` on
   * [`NotificationCompat.Builder`](https://developer.android.com/reference/androidx/core/app/NotificationCompat.Builder)
   * with the provided value.
   *
   * Only available for Android.
   *
   * @since 1.0.0
   */
  ongoing?: boolean;
  /**
   * If true, the notification is canceled when the user clicks on it.
   *
   * Calls `setAutoCancel()` on
   * [`NotificationCompat.Builder`](https://developer.android.com/reference/androidx/core/app/NotificationCompat.Builder)
   * with the provided value.
   *
   * Only available for Android.
   *
   * @since 1.0.0
   */
  autoCancel?: boolean;
  /**
   * Sets a multiline text block for display in a big text notification style.
   *
   * @since 1.0.0
   */
  largeBody?: string;
  /**
   * Used to set the summary text detail in inbox and big text notification styles.
   *
   * Only available for Android.
   *
   * @since 1.0.0
   */
  summaryText?: string;
  /**
   * Set a custom status bar icon.
   *
   * If set, this overrides the `smallIcon` option from Capacitor
   * configuration.
   *
   * Icons should be placed in your app's `res/drawable` folder. The value for
   * this option should be the drawable resource ID, which is the filename
   * without an extension.
   *
   * Only available for Android.
   *
   * @since 1.0.0
   */
  smallIcon?: string;
  /**
   * Set a large icon for notifications.
   *
   * Icons should be placed in your app's `res/drawable` folder. The value for
   * this option should be the drawable resource ID, which is the filename
   * without an extension.
   *
   * Only available for Android.
   *
   * @since 1.0.0
   */
  largeIcon?: string;
  /**
   * Specifies the channel the notification should be delivered on.
   *
   * If channel with the given name does not exist then the notification will
   * not fire. If not provided, it will use the default channel.
   *
   * Calls `setChannelId()` on
   * [`NotificationCompat.Builder`](https://developer.android.com/reference/androidx/core/app/NotificationCompat.Builder)
   * with the provided value.
   *
   * Only available for Android 26+.
   *
   * @since 1.0.0
   */
  channelId?: string;
}

/**
 * Get access to device location information.
 *
 * @since 1.0.0
 */
export interface CapacitorGeolocation {
  /**
   * Get the device's last known location
   * @since 1.0.0
   */
  getCurrentLocation: () => GetCurrentPositionResult;
}

/**
 * A simple string key / value store backed by UserDefaults on iOS and Shared Preferences on Android.
 */
export interface CapacitorKV {
  /**
   * Set a string value with the given key.
   *
   * @param key
   * @param value
   * @returns
   * @since 1.0.0
   */
  set: (key: string, value: string) => void;
  /**
   * Get a string value for the given key.
   * @param key
   * @returns
   * @since 1.0.0
   */
  get: (key: string) => string;
  /**
   * Remove a value with the given key.
   * @param key
   * @returns
   * @since 1.0.0
   */
  remove: (key: string) => void;
}

/**
 * Get information on the device, such as network connectivity and battery status.
 */
export interface CapacitorDevice {
  /**
   * Get the current battery status for the device.
   *
   * @since 1.0.0
   */
  getBatteryStatus: () => BatteryStatus;
  /**
   * Get the current network status for the device.
   *
   * @since 1.0.0
   */
  getNetworkStatus: () => NetworkStatus;
}

/**
 * Send basic local notifications.
*/
export interface CapacitorNotifications {
  /**
   * Schedule a local notification
   * @example `CapacitorNotifications.schedule()`
   * @param options
   * @returns void
   * @since 1.0.0
   */
  schedule: (options: NotificationScheduleOptions[]) => void;
}

export interface CapcacitorWatch {
  sendMessage: (options: []) => void;
  transferUserInfo: (options: []) => void;
  updateApplicationContext: (options: []) => void;
  isReachable: boolean;
  updateWatchUI: (options: { watchUI: string }) => void;
  updateWatchData: (options: { data: { [key: string]: string } }) => void;
}

export interface CapacitorAPI {
  CapacitorDevice: CapacitorDevice;
  CapacitorKV: CapacitorKV;
  CapacitorNotifications: CapacitorNotifications;
  CapacitorGeolocation: CapacitorGeolocation;
  CapacitorWatch: CapcacitorWatch;
}
