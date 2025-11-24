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
import "./Tab3.css";

const Tab3: React.FC = () => {
  const [commandOutput, setCommandOutput] = useState<string>("");

  const onCheckWatchReachable = async () => {
    setCommandOutput("");
    try {
      const result = (await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "checkWatchReachability",
        details: {},
      })) as any;

      setCommandOutput(`success: ${JSON.stringify(result)}`);
    } catch (err) {
      setCommandOutput(`ERROR: ${err}`);
    }
  };

  const onSendWearableMsg = async () => {
    setCommandOutput("");
    try {
      (await BackgroundRunner.dispatchEvent({
        label: "com.example.background.task",
        event: "sendMessageToWearable",
        details: {},
      })) as any;
      setCommandOutput("success: message sent");
    } catch (err) {
      setCommandOutput(`ERROR: ${err}`);
    }
  };

  return (
    <IonPage>
      <IonHeader>
        <IonToolbar>
          <IonTitle>Tab 3</IonTitle>
        </IonToolbar>
      </IonHeader>
      <IonContent fullscreen>
        <IonHeader collapse="condense">
          <IonToolbar>
            <IonTitle size="large">Tab 3</IonTitle>
          </IonToolbar>
        </IonHeader>
        <div>
          <IonTextarea
            id="commandOutput"
            value={commandOutput}
            autoGrow={true}
          ></IonTextarea>
        </div>
        <IonButton expand={"block"} onClick={onCheckWatchReachable}>
          Check Watch Status
        </IonButton>
        <IonButton expand={"block"} onClick={onSendWearableMsg}>
          Send Message to Watch
        </IonButton>
      </IonContent>
    </IonPage>
  );
};

export default Tab3;
