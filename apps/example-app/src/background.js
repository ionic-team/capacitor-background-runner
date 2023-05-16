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
