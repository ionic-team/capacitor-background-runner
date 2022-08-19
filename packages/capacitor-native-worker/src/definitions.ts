export interface DispatchEventOptions {
  event: string;
  details: { [key: string]: any };
}
export interface CapacitorNativeWorkerPlugin {
  dispatchEvent(options: DispatchEventOptions): Promise<void>;
}
