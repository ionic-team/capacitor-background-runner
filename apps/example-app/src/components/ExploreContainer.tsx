import { BackgroundRunner } from "@ionic-enterprise/background-runner";
import "./ExploreContainer.css";
import { IonButton } from "@ionic/react";

interface ContainerProps {
  name: string;
}

const ExploreContainer: React.FC<ContainerProps> = ({ name }) => {
  const onDispatchEvent = async () => {
    await BackgroundRunner.dispatchEvent({
      label: "com.example.background",
      event: "updateSystem",
      details: {},
    });
  };

  const onDispatchEventWithDetails = async () => {
    const result = await BackgroundRunner.dispatchEvent({
      label: "com.example.background",
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
        label: "com.example.background",
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
          label: "com.example.background",
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
        label: "com.example.background",
        event: "testKVStore",
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
        label: "com.example.background",
        event: "testSingleLocation",
        details: {},
      });
    } catch (err) {
      alert(err);
      console.error(err);
    }
  }

  return (
    <div className="container">
      <strong>{name}</strong>
      <p>
        Explore{" "}
        <a
          target="_blank"
          rel="noopener noreferrer"
          href="https://ionicframework.com/docs/components"
        >
          UI Components
        </a>
      </p>
      <IonButton onClick={onDispatchEvent}>Dispatch Event</IonButton>
      <IonButton onClick={onDispatchEventWithDetails}>
        Dispatch Event with Details
      </IonButton>
      <IonButton color="danger" onClick={onDispatchEventWithFailure}>
        Dispatch Throwing Event
      </IonButton>
      <IonButton onClick={onRegisterTask}>Register Task</IonButton>
      <IonButton onClick={onTestKV}>Test KV</IonButton>
      <IonButton onClick={onTestLocation}>Test Location</IonButton>
    </div>
  );
};

export default ExploreContainer;
