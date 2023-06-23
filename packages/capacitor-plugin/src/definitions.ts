/// <reference types="@capacitor/cli" />

import type { PermissionState } from '@capacitor/core';

export type API = 'geolocation' | 'notifications';

export interface PermissionStatus {
  geolocation: PermissionState;
  notifications: PermissionState;
}

export interface DispatchEventOptions {
  label: string;
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
  label: string;
  src: string;
  event: string;
  repeat: boolean;
  interval: number;
  autoStart?: boolean;
}

export interface BackgroundRunnerPlugin {
  checkPermissions(): Promise<PermissionStatus>;
  requestPermissions(
    options: RequestPermissionOptions,
  ): Promise<PermissionStatus>;
  dispatchEvent(options: DispatchEventOptions): Promise<void>;
  registerBackgroundTask(options: RegisterBackgroundTaskOptions): Promise<void>;
}

declare module '@capacitor/cli' {
  export interface PluginsConfig {
    BackgroundRunner?: {
      runners?: BackgroundRunnerConfig[];
    };
  }
}
