import { IonButton } from '@ionic/react';
import './ExploreContainer.css';
import { BackgroundRunner } from "@ionic-enterprise/background-runner";

interface ContainerProps {
  name: string;
}

const ExploreContainer: React.FC<ContainerProps> = ({ name }) => {
  const onClickBtn = () => {
    BackgroundRunner.init();
  }
  return (
    <div className="container">
      <strong>{name}</strong>
      <p>Explore <a target="_blank" rel="noopener noreferrer" href="https://ionicframework.com/docs/components">UI Components</a></p>
      <IonButton onClick={onClickBtn}>Init</IonButton>
    </div>
  );
};

export default ExploreContainer;
