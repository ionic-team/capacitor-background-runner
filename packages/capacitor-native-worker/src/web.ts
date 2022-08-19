import { WebPlugin } from '@capacitor/core';

import type {
  CapacitorNativeWorkerPlugin,
  DispatchEventOptions,
} from './definitions';

export class CapacitorNativeWorkerWeb
  extends WebPlugin
  implements CapacitorNativeWorkerPlugin
{
  dispatchEvent(_options: DispatchEventOptions): Promise<void> {
    throw new Error('Method not implemented.');
  }
}
