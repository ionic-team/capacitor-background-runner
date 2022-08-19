import { registerPlugin } from '@capacitor/core';

import type { CapacitorNativeWorkerPlugin } from './definitions';

const CapacitorNativeWorker = registerPlugin<CapacitorNativeWorkerPlugin>(
  'CapacitorNativeWorker',
  {
    web: () => import('./web').then(m => new m.CapacitorNativeWorkerWeb()),
  },
);

export * from './definitions';
export { CapacitorNativeWorker };
