import { WebPlugin, PluginListenerHandle } from '@capacitor/core';

import type {
  BackgroundRunnerPlugin,
  DispatchEventOptions,
  PermissionStatus,
  NotificationActionEvent,
} from './definitions';

export class BackgroundRunnerWeb
  extends WebPlugin
  implements BackgroundRunnerPlugin {
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

  async addListener(
    _eventName: 'backgroundRunnerNotificationReceived',
    _listenerFunc: (event: NotificationActionEvent) => void,
  ): Promise<PluginListenerHandle> {
    throw new Error('not available on web');
  }

  async removeNotificationListeners(): Promise<void> {
    throw new Error('not available on web');
  }
}
