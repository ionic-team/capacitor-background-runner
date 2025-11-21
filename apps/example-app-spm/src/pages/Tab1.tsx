import { useState } from "react";
import {
  IonButton,
  IonContent,
  IonHeader,
  IonPage,
  IonTextarea,
  IonTitle,
  IonToolbar,
} from "@ionic/react";
import { BackgroundRunner } from "@capacitor/background-runner";
import "./Tab1.css";

const Tab1: React.FC = () => {
  const [commandOutput, setCommandOutput] = useState<string>("");

  const onCheckPermissions = async () => {
    setCommandOutput("");
    try {
      const permissions = await BackgroundRunner.checkPermissions();
      setCommandOutput(`permissions: ${JSON.stringify(permissions)}`);
    } catch (err) {
      setCommandOutput(`ERROR: ${err}`);
    }
  };

  const onRequestPermissions = async () => {
    setCommandOutput("");
    try {
      const permissions = await BackgroundRunner.requestPermissions({
        apis: ["geolocation", "notifications"],
      });
      setCommandOutput(`permissions: ${JSON.stringify(permissions)}`);
    } catch (err) {
      setCommandOutput(`ERROR: ${err}`);
    }
  };

  const onTestCapKVSet = async () => {
    setCommandOutput("");
    try {
      await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "testCapKVSet",
        details: {
          value: "Hello World",
        },
      });
      setCommandOutput(`success: stored value 'Hello World'`);
    } catch (err) {
      setCommandOutput(`ERROR: ${err}`);
    }
  };

  const onTestCapKVGet = async () => {
    setCommandOutput("");
    try {
      const response = await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "testCapKVGet",
        details: {},
      });
      setCommandOutput(`success: retrieved ${JSON.stringify(response)}`);
    } catch (err) {
      setCommandOutput(`ERROR: ${err}`);
    }
  };

  const onTestCapKVRemove = async () => {
    setCommandOutput("");
    try {
      await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "testCapKVRemove",
        details: {},
      });
      setCommandOutput(`success: value removed`);
    } catch (err) {
      setCommandOutput(`ERROR: ${err}`);
    }
  };

  const onTestCapNotification = async () => {
    setCommandOutput("");
    try {
      await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "testCapNotification",
        details: {},
      });
      setCommandOutput(`success: notification scheduled`);
    } catch (err) {
      setCommandOutput(`ERROR: ${err}`);
    }
  };

  const onTestCapLocation = async () => {
    setCommandOutput("");
    try {
      const response = await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "testCapacitorGeolocation",
        details: {},
      });
      setCommandOutput(`success: ${JSON.stringify(response)}`);
    } catch (err) {
      setCommandOutput(`ERROR: ${err}`);
    }
  };

  const onTestCapDeviceBattery = async () => {
    setCommandOutput("");
    try {
      const response = await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "testCapacitorDeviceBatteryStatus",
        details: {},
      });
      setCommandOutput(`success: ${JSON.stringify(response)}`);
    } catch (err) {
      setCommandOutput(`ERROR: ${err}`);
    }
  };

  const onTestCapDeviceNetwork = async () => {
    setCommandOutput("");
    try {
      const response = await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "testCapacitorDeviceNetworkStatus",
        details: {},
      });
      setCommandOutput(`success: ${JSON.stringify(response)}`);
    } catch (err) {
      setCommandOutput(`ERROR: ${err}`);
    }
  };

  const onTestCapAppSetBadge = async () => {
    setCommandOutput("");
    try {
      await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "testCapacitorAppSetBadge",
        details: {},
      });
      setCommandOutput(`success`);
    } catch (err) {
      setCommandOutput(`ERROR: ${err}`);
    }
  };

  const onTestCapAppClearBadge = async () => {
    setCommandOutput("");
    try {
      await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "testCapacitorAppClearBadge",
        details: {},
      });
      setCommandOutput(`success`);
    } catch (err) {
      setCommandOutput(`ERROR: ${err}`);
    }
  };

  const onTestCapAppGetInfo = async () => {
    setCommandOutput("");
    try {
      const response = await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "testCapacitorAppGetInfo",
        details: {},
      });
      setCommandOutput(`success: ${JSON.stringify(response)}`);
    } catch (err) {
      setCommandOutput(`ERROR: ${err}`);
    }
  };

  const onTestCapAppGetState = async () => {
    setCommandOutput("");
    try {
      const response = await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "testCapacitorAppGetState",
        details: {},
      });
      setCommandOutput(`success: ${JSON.stringify(response)}`);
    } catch (err) {
      setCommandOutput(`ERROR: ${err}`);
    }
  };

  return (
    <IonPage>
      <IonHeader>
        <IonToolbar>
          <IonTitle>Capacitor APIs</IonTitle>
        </IonToolbar>
      </IonHeader>
      <IonContent fullscreen>
        <IonHeader collapse="condense">
          <IonToolbar>
            <IonTitle size="large">Capacitor APIs</IonTitle>
          </IonToolbar>
        </IonHeader>
        <div>
          <IonTextarea
            id="commandOutput"
            value={commandOutput}
            autoGrow={true}
          ></IonTextarea>
        </div>
        <IonButton
          expand="block"
          color={"success"}
          onClick={onCheckPermissions}
        >
          Check API Permissions
        </IonButton>
        <IonButton
          expand="block"
          color={"success"}
          onClick={onRequestPermissions}
        >
          Request API Permissions
        </IonButton>
        <IonButton expand="block" onClick={onTestCapKVSet}>
          Capacitor KV - Set
        </IonButton>
        <IonButton expand="block" onClick={onTestCapKVGet}>
          Capacitor KV - Get
        </IonButton>
        <IonButton expand="block" onClick={onTestCapKVRemove}>
          Capacitor KV - Delete
        </IonButton>
        <IonButton expand="block" onClick={onTestCapNotification}>
          Test Capacitor Notif.- Schedule Notification
        </IonButton>
        <IonButton expand="block" onClick={onTestCapAppSetBadge}>
          Test Capacitor Notif. - Set Badge
        </IonButton>
        <IonButton expand="block" onClick={onTestCapAppClearBadge}>
          Test Capacitor Notif. - Clear Badge
        </IonButton>
        <IonButton expand="block" onClick={onTestCapLocation}>
          Test Capacitor Geolocation
        </IonButton>
        <IonButton expand="block" onClick={onTestCapDeviceBattery}>
          Test Capacitor Device - Battery
        </IonButton>
        <IonButton expand="block" onClick={onTestCapDeviceNetwork}>
          Test Capacitor Device - Network
        </IonButton>
        <IonButton expand="block" onClick={onTestCapAppGetInfo}>
          Test Capacitor App - Get App Info
        </IonButton>
        <IonButton expand="block" onClick={onTestCapAppGetState}>
          Test Capacitor App - Get App State
        </IonButton>
      </IonContent>
    </IonPage>
  );
};

export default Tab1;
