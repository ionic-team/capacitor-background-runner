import { pathExists, readJSON, writeFile } from "@ionic/utils-fs";
import { extname, join, resolve } from "path";
import { CapacitorConfig } from "@capacitor/cli";
import { resolveNode, requireTS, formatJSObject } from "./utils";

export const CONFIG_FILE_NAME_TS = "capacitor.config.ts";
export const CONFIG_FILE_NAME_JS = "capacitor.config.js";
export const CONFIG_FILE_NAME_JSON = "capacitor.config.json";

export interface LoadedCapacitorConfig {
  configType: "ts" | "js" | "json";
  configPath: string;
  config: CapacitorConfig;
}

export const readCapConfig = async (
  projectDir: string
): Promise<LoadedCapacitorConfig> => {
  const extConfigFilePathTS = resolve(projectDir, CONFIG_FILE_NAME_TS);

  if (await pathExists(extConfigFilePathTS)) {
    const tsPath = resolveNode(projectDir, "typescript");
    if (!tsPath) {
      throw new Error("Could not find installation of TypeScript.");
    }

    const ts = require(tsPath); // eslint-disable-line @typescript-eslint/no-var-requires
    const extConfigObject = requireTS(ts, extConfigFilePathTS) as any;
    const extConfig = extConfigObject.default
      ? await extConfigObject.default
      : extConfigObject;

    return {
      configPath: extConfigFilePathTS,
      configType: "ts",
      config: extConfig,
    };
  }

  const extConfigFilePathJS = resolve(projectDir, CONFIG_FILE_NAME_JS);
  if (await pathExists(extConfigFilePathJS)) {
    return {
      configPath: extConfigFilePathJS,
      configType: "js",
      config: await require(extConfigFilePathJS),
    };
  }

  const extConfigFilePath = resolve(projectDir, CONFIG_FILE_NAME_JSON);

  return {
    configPath: extConfigFilePath,
    configType: "json",
    config: (await readJSON(extConfigFilePath)) as CapacitorConfig,
  };
};

export const writeCapConfig = async (
  loadedConfig: LoadedCapacitorConfig
): Promise<void> => {
  switch (loadedConfig.configType) {
    case "ts": {
      await writeFile(
        loadedConfig.configPath,
        formatConfigTS(loadedConfig.config)
      );
    }
  }
};

function formatConfigTS(config: CapacitorConfig): string {
  // TODO: <reference> tags
  return `import type { CapacitorConfig } from '@capacitor/cli';

const config: CapacitorConfig = ${formatJSObject(config)};

export default config;\n`;
}
