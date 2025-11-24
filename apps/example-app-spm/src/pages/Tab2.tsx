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
import "./Tab2.css";

const Tab2: React.FC = () => {
  const [commandOutput, setCommandOutput] = useState<string>("");
  const onTestSync = async () => {
    setCommandOutput("");
    try {
      await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "syncTest",
        details: {},
      });
      setCommandOutput("success");
    } catch (err) {
      setCommandOutput(`ERROR: ${err}`);
    }
  };

  const onTestAsync = async () => {
    setCommandOutput("");
    try {
      await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "asyncTest",
        details: {},
      });
      setCommandOutput("success");
    } catch (err) {
      setCommandOutput(`ERROR: ${err}`);
    }
  };

  const onTestArgs = async () => {
    setCommandOutput("");
    try {
      const response = await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "argsTest",
        details: {
          user: {
            firstName: "John",
            lastName: "Doe",
          },
        },
      });
      setCommandOutput(`success: ${JSON.stringify(response)}`);
    } catch (err) {
      setCommandOutput(`ERROR: ${err}`);
    }
  };

  const onTestErrors = async () => {
    setCommandOutput("");
    try {
      await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "errorTest",
        details: {},
      });
      setCommandOutput('success');
    } catch (err) {
      setCommandOutput(`ERROR: ${err}`);
    }
  };

  const onTestFetch = async () => {
    setCommandOutput("");
    try {
      const response = await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "fetchTest",
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
          <IonTitle>Core Functions</IonTitle>
        </IonToolbar>
      </IonHeader>
      <IonContent fullscreen>
        <IonHeader collapse="condense">
          <IonToolbar>
            <IonTitle size="large">Core Functions</IonTitle>
          </IonToolbar>
        </IonHeader>
        <div>
          <IonTextarea
            id="commandOutput"
            value={commandOutput}
            autoGrow={true}
          ></IonTextarea>
        </div>
        <IonButton expand={"block"} onClick={onTestSync}>Test Synchronous Handler</IonButton>
        <IonButton expand={"block"} onClick={onTestAsync}>Test Asynchronous Handler</IonButton>
        <IonButton expand={"block"} onClick={onTestArgs}>Test Handler with Args</IonButton>
        <IonButton expand={"block"} onClick={onTestErrors}>Test Handler with Errors</IonButton>
        <IonButton expand={"block"} onClick={onTestFetch}>Test Handler with Fetch</IonButton>
      </IonContent>
    </IonPage>
  );
};

export default Tab2;
