import { WebPlugin } from '@capacitor/core';

import type { BackgroundRunnerPlugin, DispatchEventOptions, PermissionStatus, RegisterBackgroundTaskOptions } from './definitions';

export class BackgroundRunnerWeb extends WebPlugin implements BackgroundRunnerPlugin {
  checkPermissions(): Promise<PermissionStatus> {
    throw new Error('Method not implemented.');
  }
  
  requestPermissions(): Promise<PermissionStatus> {
    throw new Error('Method not implemented.');
  }

  registerBackgroundTask(_options: RegisterBackgroundTaskOptions): Promise<void> {
    throw new Error('Method not implemented.');
  }
  
  dispatchEvent(_options: DispatchEventOptions): Promise<void> {
    throw new Error('Method not implemented.');
  }
}
