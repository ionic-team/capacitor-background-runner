// basic functionality

addEventListener("syncTest", (resolve, reject, args) => {
  console.log("calling sync test");
  setTimeout(() => {
    console.log("tick");
    resolve();
  }, 3000);
});

addEventListener("asyncTest", async (resolve, reject, args) => {
  console.log("calling async test");
  await new Promise((res) => {
    setTimeout(() => {
      console.log("tick");
      res();
    }, 3000);
  });
  console.log("returning async result");
  resolve();
});

addEventListener("argsTest", async (resolve, reject, args) => {
  try {
    console.log("accepted these args: " + JSON.stringify(args));
    const updatedUser = args.user;
    updatedUser.firstName = updatedUser.firstName + " HELLO";
    updatedUser.lastName = updatedUser.lastName + " WORLD";

    resolve(updatedUser);
  } catch (err) {
    console.error(err);
    reject(err);
  }
});

addEventListener("errorTest", async (resolve, reject, args) => {
  try {
    const undefinedObject = args.myUndefinedObject;
    undefinedObject.fakeFunc();
  } catch (err) {
    console.error(err);
    reject(err);
  }
});

addEventListener("fetchTest", async (resolve, reject, args) => {
  try {
    const res = await fetch("https://jsonplaceholder.typicode.com/todos/1");

    if (!res.ok) {
      throw new Error("Fetch GET request failed");
    }

    const todo = await res.json();
    resolve(todo);
  } catch (err) {
    console.error(err);
    reject(err);
  }
});

// capacitor APIs
addEventListener("testCapKV", async (resolve, reject, args) => {
  try {
    CapacitorKV.set("testValue", "hello world");

    const result = CapacitorKV.get("testValue");
    console.log("test value is: " + result);

    resolve(result);
  } catch (err) {
    console.error(err);
    reject(err);
  }
});

addEventListener("testCapNotification", async (resolve, reject, args) => {
  try {
    let scheduleDate = new Date();
    scheduleDate.setSeconds(scheduleDate.getSeconds() + 30);

    CapacitorNotifications.schedule([
      {
        id: 100,
        title: "Enterprise Background Runner",
        body: "A test message from the Enterprise Background Runner",
        scheduleAt: scheduleDate,
      },
    ]);

    resolve();
  } catch (err) {
    console.error(err);
    reject(err);
  }
});

addEventListener("testCapacitorGeolocation", async (resolve, reject, args) => {
  try {
    const location = await CapacitorGeolocation.getCurrentPosition();
    console.log("current location: " + JSON.stringify(location));
    resolve(location);
  } catch (err) {
    console.error(err);
    reject(err);
  }
});

addEventListener(
  "testCapacitorDeviceBatteryStatus",
  async (resolve, reject, args) => {
    try {
      const info = CapacitorDevice.getBatteryStatus();
      console.log(JSON.stringify(info));
      resolve(info);
    } catch (err) {
      console.error(err);
      reject(err);
    }
  }
);

addEventListener(
  "testCapacitorDeviceNetworkStatus",
  async (resolve, reject, args) => {
    try {
      const info = CapacitorDevice.getNetworkStatus();
      console.log(JSON.stringify(info));
      resolve(info);
    } catch (err) {
      console.error(err);
      reject(err);
    }
  }
);

addEventListener("remoteNotification", (resolve, reject, args) => {
  console.log("received silent push notification");

  CapacitorNotifications.schedule([
    {
      id: 1000,
      title: "Enterprise Background Runner",
      body: "Received silent push notification",
    },
  ]);

  console.log(`details: ${JSON.stringify(args)}`);

  resolve();
});

addEventListener("checkWatchReachability", (resolve, reject, args) => void {
  const reachable = CapacitorWatch.isReachable();

    resolve({
      reachable: reachable,
    });
  } catch (err) {
    console.error(err);
    reject(err);
  }
});

addEventListener("sendMessageToWatch", (resolve, reject, args) => void {
  console.log("sending message to watch...");

  CapacitorWatch.send({
    msg: "Hello World",
  });

    resolve();
  } catch (err) {
    console.error(err);
    reject(err);
  }
});

addEventListener(
  "WatchConnectivity_activationDidCompleteWith",
  (resolve, reject, args) => {
    console.log("watch paired completed");
    resolve();
  }
);

addEventListener(
  "WatchConnectivity_sessionDidBecomeInactive",
  (resolve, reject, args) => {
    console.log("watch session is inactive");
    resolve();
  }
);

addEventListener(
  "WatchConnectivity_sessionDidDeactivate",
  (resolve, reject, args) => {
    console.log("watch session is deactivated");
    resolve();
  }
);

addEventListener(
  "WatchConnectivity_didReceiveUserInfo",
  (resolve, reject, args) => {
    console.log(`watch sent user info: ${JSON.stringify(args)}`);
    resolve();
  }
);

addEventListener(
  "WatchConnectivity_didReceiveMessage",
  (resolve, reject, args) => {
    try {
      const msg = args.message.result;

      CapacitorNotifications.schedule([
        {
          title: "Enterprise Background Runner",
          body: msg,
        },
      ]);

      console.log(`watch sent data: ${JSON.stringify(args)}`);
      resolve();
    } catch (err) {
      console.error(err);
      reject(err);
    }
  }
);
