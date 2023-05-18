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

addEventListener("testGetKVStore", (details) => {
  const value = CapacitorKV.get("testValue");

  CapacitorKV.set("testValue", null);

  details.completed({
    value
  });
})

addEventListener("testLastKnownLocation", async (details) => {
  const location = CapacitorGeolocation.getCurrentPosition();

  console.log("current location: " + JSON.stringify(location));

  CapacitorNotifications.schedule({
    title: "Enterprise Background Runner",
    body: `Your current location: ${location.lat}, ${location.lng}`
  });

  CapacitorKV.set("testValue", JSON.stringify(location));

  details.completed();
});

addEventListener("testCurrentLocation", async (details) => {
  console.log("getting current location...");
  const location = await CapacitorGeolocation.getCurrentPosition();

  console.log("current location: " + JSON.stringify(location));
  details.completed();
});

addEventListener("testStartLocationWatch", (details) => {
  CapacitorGeolocation.startWatchingPosition(false);

  details.completed();
});

addEventListener("testEndLocationWatch", (details) => {
  CapacitorGeolocation.stopWatchingPosition();

  details.completed();
});

addEventListener("currentLocation", (details) => {
  console.log("current live location: " + JSON.stringify(details.locations));

  details.completed();
});

addEventListener("scheduleNotification", (details) => {
  CapacitorNotifications.schedule({
    title: "Enterprise Background Runner",
    body: "A test message from the Enterprise Background Runner"
  });
  details.completed();
});

addEventListener("monitorLocation", async (details) => {
  console.log("recording location...")
  const location = await CapacitorGeolocation.getCurrentPosition();
  const timestamp = Math.floor(Date.now() / 1000);

  let track = CapacitorKV.get("track");
  if (!track) {
    track = [];
  } else {
    track = JSON.parse(track);
  }

  track.push({
    timestamp,
    location,
  });

  CapacitorKV.set("track", JSON.stringify(track));

  CapacitorNotifications.schedule({
    title: "Enterprise Background Runner",
    body: "Recording your current location"
  });

  details.completed();
});

addEventListener("getSavedLocations", (details) => {
  let track = CapacitorKV.get("track");

  details.completed({ track: track });
});
