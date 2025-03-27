import { registerPlugin } from '@capacitor/core';

import type { BackgroundRunnerPlugin } from './definitions';

const BackgroundRunner = registerPlugin<BackgroundRunnerPlugin>(
  'BackgroundRunner',
  {
    web: () => import('./web').then(m => new m.BackgroundRunnerWeb()),
  },
);

export * from './definitions';

export { BackgroundRunner };
