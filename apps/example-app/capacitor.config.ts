import { CapacitorConfig } from "@capacitor/cli";

const config: CapacitorConfig = {
  appId: "io.ionic.starter",
  appName: "example-app",
  webDir: "build",
  bundledWebRuntime: false,
  plugins: {
    BackgroundRunner: {
      label: "com.example.background.task",
      src: "background.js",
      event: "monitorLocation",
      repeat: true,
      interval: 2,
      autoStart: false,
    },
  },
};

export default config;
