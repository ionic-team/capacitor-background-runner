import prompts from "prompts";
import { API } from "@capacitor/background-runner";

import { availableAPIs } from "../definitions";
import { readCapConfig } from "../utils";

export const setup = async (): Promise<void> => {
  console.log(process.cwd());

  const capConfig = readCapConfig(process.cwd());

  const existingAPIs = capConfig.plugins?.BackgroundRunner?.apis as API[];
  console.log(existingAPIs);

  const response = await prompts({
    type: "multiselect",
    name: "apis",
    message: "Select the APIs you want to use",
    choices: availableAPIs,
    hint: "- Space to select. Return to submit",
  });

  console.log(response);
};
