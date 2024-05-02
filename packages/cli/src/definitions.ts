import prompts from "prompts";

export const availableAPIs: prompts.Choice[] = [
  { title: "Geolocation", description: "", value: "geolocation" },
  { title: "Notifications", value: "notifications" },
  { title: "Key / Value Store", value: "kv" },
  { title: "Device", value: "device" },
];
