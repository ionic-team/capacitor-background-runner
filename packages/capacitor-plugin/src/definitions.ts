/// <reference types="@capacitor/cli" />

import type { PermissionState } from '@capacitor/core';

export interface BackgroundRunnerConfig {
  /**
   * The name of the runner, used in logs.
   *
   * @since 1.0.0
   */
  label: string;
  /**
   * The path to the runner JavaScript file, relative to the app bundle.
   *
   * @since 1.0.0
   */
  src: string;
  /**
   * The name of the event that will be called when the OS executes the background task.
   *
   * @since 1.0.0
   */
  event: string;
  /**
   * If background task should repeat based on the interval set in `interval`.
   *
   * @since 1.0.0
   */
  repeat: boolean;
  /**
   * The number of minutes after the the app is put into the background in which the background task should begin.
   *
   * If `repeat` is true, this also specifies the number of minutes between each execution.
   *
   * @since 1.0.0
   */
  interval: number;
  /**
   * Automatically register and schedule background task on app load.
   *
   * @since 1.0.0
   */
  autoStart?: boolean;
}

declare module '@capacitor/cli' {
  export interface PluginsConfig {
    /**
     * On load, Background Runner will automatically register a
     * background task that will be scheduled and ran once your app is
     * backgrounded.
     */
    BackgroundRunner?: {
      /**
       * The name of the runner, used in logs.
       *
       * @since 1.0.0
       * @example "com.example.background.task"
       */
      label: string;
      /**
       * The path to the runner JavaScript file, relative to the app bundle.
       *
       * @since 1.0.0
       * @example "runners/background.js"
       */
      src: string;
      /**
       * The name of the event that will be called when the OS executes the background task.
       *
       * @since 1.0.0
       * @example "myCustomEvent"
       */
      event: string;
      /**
       * If background task should repeat based on the interval set in `interval`.
       *
       * @since 1.0.0
       * @example true
       */
      repeat: boolean;
      /**
       * The number of minutes after the the app is put into the background in which the background task should begin.
       *
       * If `repeat` is true, this also specifies the number of minutes between each execution.
       *
       * @since 1.0.0
       * @example 15
       */
      interval: number;
      /**
       * Automatically register and schedule background task on app load.
       *
       * @since 1.0.0
       * @example true
       */
      autoStart?: boolean;
    };
  }
}

export type API = 'geolocation' | 'notifications';

export interface PermissionStatus {
  geolocation: PermissionState;
  notifications: PermissionState;
}

export interface DispatchEventOptions {
  /**
   * The runner label to dispatch the event to
   *
   * @since 1.0.0
   */
  label: string;
  /**
   * The name of the registered event listener.
   *
   * @since 1.0.0
   */
  event: string;
  details: { [key: string]: any };
}

export interface RequestPermissionOptions {
  apis: API[];
}

export interface RegisterBackgroundTaskOptions {
  runner: BackgroundRunnerConfig;
}

export interface BackgroundRunnerPlugin {
  /**
   * Check permissions for the various Capacitor device APIs.
   *
   * @since 1.0.0
   */
  checkPermissions(): Promise<PermissionStatus>;
  /**
   * Request permission to display local notifications.
   *
   * @since 1.0.0
   */
  requestPermissions(
    options: RequestPermissionOptions,
  ): Promise<PermissionStatus>;
  /**
   * Dispatches an event to the configured runner.
   * @param options
   * @since 1.0.0
   */
  dispatchEvent<T = void>(options: DispatchEventOptions): Promise<T>;
}

declare module '@capacitor/cli' {
  export interface PluginsConfig {
    BackgroundRunner?: BackgroundRunnerConfig;
  }
}
