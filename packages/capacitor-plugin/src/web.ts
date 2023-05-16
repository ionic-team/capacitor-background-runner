import { WebPlugin } from '@capacitor/core';

import type { BackgroundRunnerPlugin, DispatchEventOptions, RegisterBackgroundTaskOptions } from './definitions';

export class BackgroundRunnerWeb extends WebPlugin implements BackgroundRunnerPlugin {
  registerBackgroundTask(_options: RegisterBackgroundTaskOptions): Promise<void> {
    throw new Error('Method not implemented.');
  }
  
  dispatchEvent(_options: DispatchEventOptions): Promise<void> {
    throw new Error('Method not implemented.');
  }
}
