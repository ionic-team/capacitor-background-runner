import { WebPlugin } from '@capacitor/core';

import type { BackgroundRunnerPlugin, DispatchEventOptions } from './definitions';

export class BackgroundRunnerWeb extends WebPlugin implements BackgroundRunnerPlugin {
  dispatchEvent(_options: DispatchEventOptions): Promise<void> {
    throw new Error('Method not implemented.');
  }
}
