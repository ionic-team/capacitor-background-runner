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
    console.log(result);
  };
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
    </div>
  );
};

export default ExploreContainer;
