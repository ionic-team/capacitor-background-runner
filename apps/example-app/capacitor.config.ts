import { CapacitorConfig } from '@capacitor/cli';

const config: CapacitorConfig = {
  appId: 'io.ionic.starter',
  appName: 'example-app',
  webDir: 'build',
  bundledWebRuntime: false,
  plugins: {
    "BackgroundRunner": {
      "runners": [
        {
          label: "com.example.background",
          src: "background.js",
          event: "updateSystem",
          repeat: false,
          interval: "15m",
          // TODO: Add Android / iOS constraints
        }
      ]
    }
  }
};

export default config;
