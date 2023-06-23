import { BackgroundRunner } from "@ionic-enterprise/background-runner";
import "./ExploreContainer.css";
import { IonButton } from "@ionic/react";

interface ContainerProps {
  name: string;
}

const ExploreContainer: React.FC<ContainerProps> = ({ name }) => {
  const onDispatchEvent = async () => {
    await BackgroundRunner.dispatchEvent({
      label: "com.example.background.task",
      event: "updateSystem",
      details: {},
    });
  };

  const onDispatchEventWithDetails = async () => {
    const result = await BackgroundRunner.dispatchEvent({
      label: "com.example.background.task",
      event: "updateSystemWithDetails",
      details: {
        user: {
          firstName: "John",
          lastName: "Doe",
        },
      },
    });
    alert(JSON.stringify(result))
    console.log(result);
  };

  const onDispatchEventWithFailure = async () => {
    try {
      await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "updateSystemThrow",
        details: {},
      });
    } catch (err) {
      alert(err);
      console.error(err);
    }
  };

  const onRegisterTask = async () => {
    try {
      await BackgroundRunner.registerBackgroundTask({
        runner: {
          label: "com.example.background.task",
          src: "background.js",
          event: "updateSystem",
          repeat: false,
          interval: 2,
        },
      });
    } catch (err) {
      alert(err);
      console.error(err);
    }
  };

  const onTestKV = async () => {
    try {
      await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "testKVStore",
        details: {},
      });
    } catch (err) {
      alert(err);
      console.error(err);
    }
  }

  const onTestGetKV = async () => {
    try {
      const result = await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "testGetKVStore",
        details: {},
      });

      alert(JSON.stringify(result));
    } catch (err) {
      alert(err);
      console.error(err);
    }
  }

  const onTestNotification = async () => {
    try {
      await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "scheduleNotification",
        details: {},
      });
    } catch (err) {
      alert(err);
      console.error(err);
    }
  }

  const onTestLocation = async () => {
    try {
      await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "testLastKnownLocation",
        details: {},
      });
    } catch (err) {
      alert(err);
      console.error(err);
    }
  }

  const onTestCurrentLocation = async () => {
    try {
      await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "testCurrentLocation",
        details: {},
      });
    } catch (err) {
      alert(err);
      console.error(err);
    }
  }

  const onTestStartLiveLocation = async () => {
    try {
      await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "testStartLocationWatch",
        details: {},
      });
    } catch (err) {
      alert(err);
      console.error(err);
    }
  }

  const onTestStopLiveLocation = async () => {
    try {
      await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "testEndLocationWatch",
        details: {},
      });
    } catch (err) {
      alert(err);
      console.error(err);
    }
  }

  const onMonitorLocation = async () => {
    try {
      await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "monitorLocation",
        details: {},
      });
    } catch (err) {
      alert(err);
      console.error(err);
    }
  }

  const onGetTrackedLocations = async () => {
    try {
      const result = await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "getSavedLocations",
        details: {},
      }) as any;

      const track = JSON.parse(result.track);

      console.log(track)

      alert(`${track.length} tracked locations`);
    } catch (err) {
      alert(err);
      console.error(err);
    }
  }

  const onCheckWatchReachable = async() => {
    try {
      const result = await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "checkWatchReachability",
        details: {},
      }) as any;      

      alert(JSON.stringify(result));
    } catch (err) {
      alert(err);
      console.error(err);
    }
  }

  const onSendWearableMsg = async() => {
    try {
      await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "sendMessageToWearable",
        details: {},
      }) as any;      
    } catch (err) {
      alert(err);
      console.error(err);
    }
  }

  const onCheckPermissions = async() => {
    try {
      const permissions = await BackgroundRunner.checkPermissions();
      alert(JSON.stringify(permissions));
    } catch (err) {
      alert(err);
      console.error(err);
    }
  }

  const onRequestPermissions = async() => {
    try {
      const permissions = await BackgroundRunner.requestPermissions({
        apis: ["geolocation", "notifications"]
      });

      alert(JSON.stringify(permissions));
    } catch (err) {
      alert(err);
      console.error(err);
    }
  }

  const onTestBatteryStatus = async() => {
    try {
      const result = await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "testBatteryStatus",
        details: {},
      }) as any;      

      alert(JSON.stringify(result));
    } catch (err) {
      alert(err);
      console.error(err);
    }
  }

  const onTestNetworkStatus = async() => {
    try {
      const result = await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "testNetworkStatus",
        details: {},
      }) as any;      

      alert(JSON.stringify(result));
    } catch (err) {
      alert(err);
      console.error(err);
    }
  }

  return (
    <div className="container">
      <IonButton onClick={onCheckPermissions}>Check API Permissions</IonButton>
      <IonButton onClick={onRequestPermissions}>Request API Permissions</IonButton>
      <IonButton onClick={onDispatchEvent}>Dispatch Event</IonButton>
      <IonButton onClick={onDispatchEventWithDetails}>
        Dispatch Event with Details
      </IonButton>
      <IonButton color="danger" onClick={onDispatchEventWithFailure}>
        Dispatch Throwing Event
      </IonButton>
      <IonButton onClick={onRegisterTask}>Register Task</IonButton>
      <IonButton onClick={onTestKV}>Test KV</IonButton>
      <IonButton onClick={onTestGetKV}>Test Get KV</IonButton>
      <IonButton onClick={onTestNotification}>Test Notification</IonButton>
      <IonButton onClick={onTestLocation}>Get Last Known Location</IonButton>
      <IonButton onClick={onTestCurrentLocation}>Get Current Location</IonButton>
      <IonButton onClick={onTestStartLiveLocation}>Start Live Location</IonButton>
      <IonButton onClick={onTestStopLiveLocation}>Stop Live Location</IonButton>
      <IonButton onClick={onMonitorLocation}>Record Location</IonButton>
      <IonButton onClick={onGetTrackedLocations}>Get Location Report</IonButton>
      <IonButton onClick={onCheckWatchReachable}>Check Watch Status</IonButton>
      <IonButton onClick={onSendWearableMsg}>Send Message to Watch</IonButton>
      <IonButton onClick={onTestBatteryStatus}>Get Battery Status</IonButton>
      <IonButton onClick={onTestNetworkStatus}>Get Network Status</IonButton>
      
    </div>
  );
};

export default ExploreContainer;
