import { Command } from "commander";

import { setup } from "./commands/setup"

const program = new Command();

program
  .command("setup")
  .description("Configure your native projects for Background Runner")
  .action(() => {
    setup();
  });

program
  .command("sync")
  .description("Perform steps to configure native projects with the configured Background Runner APIs")
  .action(() => {
    console.log("sync br config here");
  })

program.parse(process.argv);
