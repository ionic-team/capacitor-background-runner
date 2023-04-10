/// <reference types="@capacitor/cli" />

export interface DispatchEventOptions {
  event: string;
  details: { [key: string]: any };
}

export interface BackgroundRunnerConfig {
  label: string;
  src: string;
  event: string;
  repeat: boolean;
  interval: number;
}

export interface BackgroundRunnerPlugin {
  init(): Promise<void>;
  dispatchEvent(options: DispatchEventOptions): Promise<void>;
}


declare module '@capacitor/cli' {
  export interface PluginsConfig {
    BackgroundRunner?: {
      runners?: BackgroundRunnerConfig[]
    }
  }
}