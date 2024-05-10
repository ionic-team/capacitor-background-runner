import prompts from "prompts";
import { API } from "@capacitor/background-runner";

import { availableAPIs } from "../definitions";
import { readCapConfig, writeCapConfig } from "../config";

export const setup = async (): Promise<void> => {
  console.log(process.cwd());

  const loadedConfig = await readCapConfig(process.cwd());

  let existingConfig = loadedConfig.config.plugins?.BackgroundRunner;

  const response = await prompts([
    {
      type: "text",
      name: "label",
      initial: existingConfig?.label,
      message: "Enter your runner label",
      instructions: "The name of the runner, used in logs.",
      validate: (value) => (value ? true : "A runner name is required."),
    },
    {
      type: "text",
      name: "src",
      initial: existingConfig?.src,
      message:
        "Enter the path to your runner JavaScript file (relative to the app bundle)",
      instructions:
        "The path to the runner JavaScript file, relative to the app bundle.",
      validate: (value) =>
        value ? true : "The JS runner source path is required.",
    },
    {
      type: "text",
      name: "event",
      initial: existingConfig?.event,
      message: "Enter the event name",
      instructions:
        "The name of the event that will be called when the OS executes the background task.",
    },
    {
      type: "number",
      name: "interval",
      initial: existingConfig?.interval,
      message: "Enter the delay (in minutes)",
      instructions:
        "The number of minutes after the the app is put into the background in which the background task should begin.",
      validate: (value) =>
        value >= 15
          ? true
          : "Interval must be at least 15 minutes.  For more info, visit: https://capacitorjs.com/docs/apis/background-runner#limitations-of-background-tasks",
    },
    {
      type: "toggle",
      initial: existingConfig?.repeat,
      name: "repeat",
      message: "Repeat the background task on an interval?",
      instructions: "If background task should repeat based on the interval",
    },
    {
      type: "toggle",
      name: "autoStart",
      initial: existingConfig?.autoStart,
      message: "Automatically schedule and register background task?",
    },
    {
      type: "multiselect",
      name: "apis",
      message: "Select the APIs you want to use",
      choices: availableAPIs.map((a) => {
        if (existingConfig?.apis && existingConfig.apis?.includes(a.value)) {
          a.selected = true;
        }
        return a;
      }),
      hint: "- Space to select. Return to submit",
    },
  ]);

  if (!loadedConfig.config.plugins) {
    loadedConfig.config.plugins = {};
  }

  loadedConfig.config.plugins.BackgroundRunner = response;

  await writeCapConfig(loadedConfig);
};
