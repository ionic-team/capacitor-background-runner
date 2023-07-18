export interface GetCurrentPositionResult {
  latitude: number;
  longitude: number;
  accuracy: number;
  altitude: number;
  altitudeAccuracy?: number;
  speed: number;
  heading: number;
}

export interface BatteryStatus {
  batteryLevel: number;
  isCharging: boolean;
}

export interface NetworkStatus {
  connected: boolean;
  connectionType: string;
}

export interface NotificationScheduleOptions {
  id: number;
  title: string;
  body: string;
  scheduleAt: Date;
  sound?: string;
  actionTypeId?: string;
  threadIdentifier?: string;
  summaryArgument?: string;
  groupSummary?: string;
  extra?: unknown;
  ongoing?: boolean;
  autoCancel?: boolean;
  largeBody?: string;
  summaryText?: string;
  smallIcon?: string;
  largeIcon?: string;
  group?: string;
  channelId?: string;
}

export interface CapacitorGeolocation {
  getCurrentLocation: () => GetCurrentPositionResult;
}

export interface CapacitorKV {
  set: (key: string, value: string) => void;
  get: (key: string) => string;
  remove: (key: string) => void;
}

export interface CapacitorDevice {
  getBatteryStatus: () => BatteryStatus;
  getNetworkStatus: () => NetworkStatus;
}

export interface CapacitorNotifications {
  schedule: (options: NotificationScheduleOptions[]) => void;
}
