import { registerPlugin } from '@capacitor/core';

import type { BackgroundRunnerPlugin } from './definitions';

const BackgroundRunner = registerPlugin<BackgroundRunnerPlugin>('CapacitorBackgroundRunner', {
  web: () => import('./web').then((m) => new m.BackgroundRunnerWeb()),
});

export * from './definitions';
export * from './apis';

export { BackgroundRunner };
