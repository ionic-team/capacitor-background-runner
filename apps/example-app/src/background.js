addEventListener("updateSystem", (details) => {
  console.log("do something to update the system here");
  details.completed();
});

addEventListener("updateSystemWithDetails", (details) => {
  console.log("accepted this data: " + JSON.stringify(details.user));
  const updatedUser = details.user;
  updatedUser.firstName = updatedUser.firstName + " HELLO";
  updatedUser.lastName = updatedUser.lastName + " WORLD";

  details.completed(updatedUser);
});

addEventListener("updateSystemThrow", () => {
  throw new Error("an error was thrown from javascript");
});

addEventListener("testKVStore", (details) => {
  CapacitorKV.set("testValue", "hello world");
  const result = CapacitorKV.get("testValue");

  console.log("test value is: " + result);

  details.completed();
});

addEventListener("testLastKnownLocation", async (details) => {
  const location = CapacitorGeolocation.getLastPosition();

  console.log("current location: " + JSON.stringify(location));

  details.completed();
});

addEventListener("testCurrentLocation", async (details) => {
  console.log("getting current location...");
  const location = await CapacitorGeolocation.getCurrentPosition();

  console.log("current location: " + JSON.stringify(location));
  details.completed();
});

addEventListener("testStartLocationWatch", (details) => {
  CapacitorGeolocation.startWatchingPosition();

  details.completed();
});

addEventListener("testEndLocationWatch", (details) => {
  CapacitorGeolocation.stopWatchingPosition();

  details.completed();
});

addEventListener("currentLocation", (details) => {
  console.log("current live location: " + JSON.stringify(details.locations))

  details.completed();
});