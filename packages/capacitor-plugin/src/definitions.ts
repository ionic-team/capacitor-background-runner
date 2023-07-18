/// <reference types="@capacitor/cli" />

import type { PermissionState } from '@capacitor/core';

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

export interface BackgroundRunnerConfig {
  /**
   * The name of the runner, used in logs.
   */
  label: string;
  /**
   * The path to the runner JavaScript file, relative to the app bundle.
   */
  src: string;
  /**
   * The custom event that will be called when the background task is run.
   */
  event: string;
  /**
   * Repeats the execution of the background task based on the interval set in `interval`.
   */
  repeat: boolean;
  /**
   * The number of minutes after the the app is put into the background in which the background task should begin.  
   * 
   * If `repeat` is true, this also specifies the number of minutes between each execution.
   */
  interval: number;
  /**
   * Automatically register and schedule background task on app load.
   */
  autoStart?: boolean;
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
  dispatchEvent(options: DispatchEventOptions): Promise<void>;
}

declare module '@capacitor/cli' {
  export interface PluginsConfig {
    BackgroundRunner?: BackgroundRunnerConfig
  }
}


