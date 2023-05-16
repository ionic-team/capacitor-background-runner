/// <reference types="@capacitor/cli" />

export interface DispatchEventOptions {
  label: string;
  event: string;
  details: { [key: string]: any };
}

export interface RegisterBackgroundTaskOptions {
  runner: BackgroundRunnerConfig
}

export interface BackgroundRunnerConfig {
  label: string;
  src: string;
  event: string;
  repeat: boolean;
  interval: number;
  autoStart?: boolean
}

export interface BackgroundRunnerPlugin {
  dispatchEvent(options: DispatchEventOptions): Promise<void>;
  registerBackgroundTask(options: RegisterBackgroundTaskOptions): Promise<void>;
}


declare module '@capacitor/cli' {
  export interface PluginsConfig {
    BackgroundRunner?: {
      runners?: BackgroundRunnerConfig[]
    }
  }
}