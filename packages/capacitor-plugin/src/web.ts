import { WebPlugin } from '@capacitor/core';

import type { BackgroundRunnerPlugin, DispatchEventOptions, PermissionStatus } from './definitions';

export class BackgroundRunnerWeb
  extends WebPlugin
  implements BackgroundRunnerPlugin
{
  checkPermissions(): Promise<PermissionStatus> {
    throw new Error('not available on web');
  }

  requestPermissions(): Promise<PermissionStatus> {
    throw new Error('not available on web');
  }

  registerBackgroundTask(): Promise<void> {
    throw new Error('not available on web');
  }

  dispatchEvent<T = void>(_options: DispatchEventOptions): Promise<T> {
    throw new Error('not available on web');
  }  
}
